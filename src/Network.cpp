#include "Network.hpp"

using namespace std::chrono_literals;

Seer::Network::~Network()
{
	_destory = true;
	_hearbeat.join();
	try
	{
		//wait for all futures	
		for (auto& task : _tasks)
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

void Seer::Network::heartbeat()
{
	while (_destory == false)
	{
		try
		{
			auto start_of_heartbeat = std::chrono::steady_clock::now();
			//remove any tasks that have completed, or thrown errors
			_tasks.erase(
				std::remove_if(_tasks.begin(), _tasks.end(), [this](auto& task) {return task_complete(task); })
				, _tasks.end());

			//swap out the mutexed vector to a local one so we don't block it for long
			std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> data_points_to_send;
			{
				std::lock_guard<std::mutex> guard(_data_point_mutex);
				std::swap(data_points_to_send, _data_points);
			}
			if (data_points_to_send.size() > 0)
			{
				//transform all the data points to json
				nlohmann::json json = nlohmann::json::array();
				std::vector<nlohmann::json> jsonV;
				jsonV.reserve(data_points_to_send.size());
				for (const auto& data_point : data_points_to_send)
				{
					jsonV.push_back(data_point->get_json());
				}
				std::cout << jsonV;
			}
			//Convert json to string
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
	_server.set_message_handler([this](auto handle, auto message) {
		std::cout << message->get_payload() << std::endl;
	});

	_server.init_asio();
	_server.listen(9002);
	_server.start_accept();

	_server.run();
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
