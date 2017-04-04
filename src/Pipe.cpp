#include "Pipe.hpp"

using namespace std::chrono_literals;

Seer::Pipe::~Pipe()
{
	_destory = true;
	//_received_messages_condition.notify_one();
	//_hearbeat.join();
	//_message_process_thread.join();
	//try
	//{
	//	//wait for all futures	
	//	for (auto& task : _running_tasks)
	//	{
	//		task.wait();
	//	}
	//}
	//catch (const std::exception&)
	//{

	//}
}

Seer::Pipe::Pipe()
{
	_hearbeat = std::thread([this]() { heartbeat(); });
}

void Seer::Pipe::send(std::unique_ptr<DataPoint::BaseDataPoint> time_point)
{
	std::lock_guard<std::mutex> guard(_data_point_mutex);
	_data_points.push_back(std::move(time_point));
}

void Seer::Pipe::heartbeat()
{
	while (_destory == false)
	{
		try
		{
			const auto start_of_heartbeat = std::chrono::steady_clock::now();
			//swap out the mutexed vector to a local one so we don't block it for long
			std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> data_points_to_send;
			{
				std::lock_guard<std::mutex> guard(_data_point_mutex);
				std::swap(data_points_to_send, _data_points);
			}
			const auto sinks_active = std::any_of(_sinks.begin(), _sinks.end(), [](auto sink) { return sink.active(); });
			//Parse the data points in to json
			if (sinks_active && data_points_to_send.size() > 0)
			{
				std::stringstream json_stream;
				for (const auto& data_point : data_points_to_send)
				{
					json_stream << *data_point;
				}
				const auto prep_time = std::chrono::steady_clock::now();
				const auto json_string = json_stream.str();
				for (auto& sink : _sinks)
				{
					sink.send(json_string);
				}
				//send_to_clients(json_stream.str());
				const auto send_time = std::chrono::steady_clock::now();
				/*std::cout << "Time to send [" << data_points_to_send.size() << "] datapoints\n";
				std::cout << "Prep: " << std::chrono::duration_cast<std::chrono::milliseconds>(prep_time - start_of_heartbeat).count() << "ms \n";
				std::cout << "Send: " << std::chrono::duration_cast<std::chrono::milliseconds>(send_time - prep_time).count() << "ms \n";*/
			}
			std::this_thread::sleep_until(start_of_heartbeat + pipe_heartbeat{ 1 });
		}
		catch (const std::exception&)
		{

		}
	}
}