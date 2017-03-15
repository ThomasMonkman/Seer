#include "Network.hpp"

using namespace std::chrono_literals;

Seer::Network::~Network()
{
	_destory = true;
	_received_messages_condition.notify_one();
	_hearbeat.join();
	_message_process_thread.join();
	try
	{
		//wait for all futures	
		for (auto& task : _running_tasks)
		{
			task.wait();
		}
	}
	catch (const std::exception&)
	{

	}
}

void Seer::Network::send(std::unique_ptr<DataPoint::BaseDataPoint> time_point)
{
	std::lock_guard<std::mutex> guard(_data_point_mutex);
	_data_points.push_back(std::move(time_point));
}

void Seer::Network::add_listener(std::unique_ptr<Listener::BaseListener> listener)
{
	std::lock_guard<std::mutex> guard(_task_mutex);
	listener->
	_task_listeners[listener->get_name()] = std::move(listener);
}

void Seer::Network::heartbeat()
{
	while (_destory == false)
	{
		try
		{
			auto start_of_heartbeat = std::chrono::steady_clock::now();
			//remove any tasks that have completed, or thrown errors
			{
				std::lock_guard<std::mutex> guard(_task_mutex);
				_running_tasks.erase(
					std::remove_if(_running_tasks.begin(), _running_tasks.end(), [this](auto& task) {return task_complete(task); })
					, _running_tasks.end());
			}

			//swap out the mutexed vector to a local one so we don't block it for long
			std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> data_points_to_send;
			{
				std::lock_guard<std::mutex> guard(_data_point_mutex);
				std::swap(data_points_to_send, _data_points);
			}
			if (data_points_to_send.size() > 0)
			{
				std::stringstream json_stream;
				for (const auto& data_point : data_points_to_send)
				{
					json_stream << *data_point;
				}
				auto prep_time = std::chrono::steady_clock::now();
				send_to_clients(json_stream.str());
				auto send_time = std::chrono::steady_clock::now();
				std::cout << "Time to send [" << data_points_to_send.size() << "] datapoints\n";
				std::cout << "Prep: " << std::chrono::duration_cast<std::chrono::milliseconds>(prep_time - start_of_heartbeat).count() << "ms \n";
				std::cout << "Send: " << std::chrono::duration_cast<std::chrono::milliseconds>(send_time - prep_time).count() << "ms \n";
			}
			std::this_thread::sleep_until(start_of_heartbeat + network_heartbeat{ 1 });
		}
		catch (const std::exception&)
		{
			consume_exception(std::current_exception());
		}
	}
}

void Seer::Network::start_server()
{
	// disable loggin for the websocket server
	_server.clear_access_channels(websocketpp::log::alevel::all);
	_server.set_open_handler([this](auto connection) {
		std::lock_guard<std::mutex> guard(_connection_mutex);		
		_connections.insert(connection);
	});
	_server.set_close_handler([this](auto connection) {
		std::lock_guard<std::mutex> guard(_connection_mutex);
		auto found = _connections.find(connection);
		if (found != _connections.end())
		{
			_connections.erase(found);
		}
	});
	_server.set_message_handler([this](auto connection, auto message) {
		try
		{
			std::lock_guard<std::mutex> guard(_received_messages_mutex);
			_received_messages.push(message->get_payload());
			_received_messages_condition.notify_one();
		}
		catch (const std::exception&)
		{
			consume_exception(std::current_exception());
		}
	});

	_server.init_asio();
	_server.listen(9002);
	_server.start_accept();

	_server_thread = std::thread([this]() { _server.run(); });
	//_server.run();
}

void Seer::Network::send_to_clients(const std::string& message)
{
	std::lock_guard<std::mutex> guard(_connection_mutex);
	for (auto& connection : _connections) 
	{
		std::cout << "Message is " << message.size() / 1'000'000.0 << "MB\n";
		_server.send(connection, message, websocketpp::frame::opcode::text);
	}	
}

void Seer::Network::received_messages_worker()
{
	while (_destory == false)
	{
		std::unique_lock<std::mutex> lock(_received_messages_mutex);
		_received_messages_condition.wait(lock, [this] { return _received_messages.empty() == false; });

		std::queue<std::string> received_messages;
		std::swap(_received_messages, received_messages);

		lock.unlock();

		while (received_messages.empty() == false)
		{
			auto message = received_messages.front();
			received_messages.pop();
			process_received_messages(message);
		}
	}
}

void Seer::Network::process_received_messages(const std::string& message)
{
	std::cout << message << std::endl;
	//check if the message is json
	try
	{
		auto json = nlohmann::json::parse(message.begin(), message.end());
		std::cout << json << std::endl;
		{
			std::lock_guard<std::mutex> guard(_task_mutex);
			auto listener = _task_listeners.find(json["n"]);
			if (listener != _task_listeners.end())
			{
				std::packaged_task<void()> task(listener->second->get_callback());
				_running_tasks.push_back(task.get_future());
				std::thread(std::move(task)).detach();
			}
		}
	}
	catch (const std::exception&)
	{
		consume_exception(std::current_exception());		
	}
	
}

bool Seer::Network::task_complete(std::future<void>& task)
{
	auto task_status = task.wait_for(0s);
	if (task_status == std::future_status::ready)
	{
		try
		{
			if (task.valid())
			{
				task.get();
				return true;
			}
		}
		catch (const std::exception&)
		{
			consume_exception(std::current_exception());
			return true;
		}
	}
	return false;
}


void Seer::Network::consume_exception(std::exception_ptr&& exception)
{
	std::lock_guard<std::mutex> guard(_exception_mutex);
	_exceptions_caught.push_back(exception);
	_exception_has_been_raised = true;
}
