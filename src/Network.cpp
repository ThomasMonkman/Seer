#include "Network.hpp"

using namespace std::chrono_literals;

Seer::Network::~Network()
{
	//wait for all futures
}

void Seer::Network::send(std::unique_ptr<DataPoint::BaseDataPoint> time_point)
{
	std::lock_guard<std::mutex> guard(_send_mutex);
	_time_points.push_back(std::move(time_point));
}

void Seer::Network::heartbeat()
{
	try
	{
		//remove any tasks that have completed, or thrown errors
		_tasks.erase(std::remove_if(_tasks.begin(), _tasks.end(), task_complete), _tasks.end());
	}
	catch (const std::exception&)
	{
		std::lock_guard<std::mutex> guard(_exception_mutex);
		_exceptions_caught.push_back(std::current_exception());
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
		catch (const std::exception& e)
		{
			std::lock_guard<std::mutex> guard(_exception_mutex);
			_exceptions_caught.push_back(std::current_exception());
			return true;
		}
	}
	return false;
}
