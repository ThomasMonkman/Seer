#include "Network.hpp"

using namespace std::chrono_literals;

Seer::Network::~Network()
{
	//wait for all futures	
	for (auto& task : _tasks)
	{
		task.wait();
	}
}

void Seer::Network::send(std::unique_ptr<DataPoint::BaseDataPoint> time_point)
{
	std::lock_guard<std::mutex> guard(_send_mutex);
	_time_points.push_back(std::move(time_point));
	if (_exception_has_been_raised)
	{
		std::lock_guard<std::mutex> guard(_exception_mutex);
		if (_exceptions_caught.size() > 0) {
			std::exception_ptr exp = std::move(_exceptions_caught.back());
			_exceptions_caught.erase(_exceptions_caught.end() - 1);
			//keep the flag raised if there are more exceptions to throw
			_exception_has_been_raised = (_exceptions_caught.size() > 0);
			std::rethrow_exception(exp);
		}
	}
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
		_exception_has_been_raised = true;
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
			_exception_has_been_raised = true;
			return true;
		}
	}
	return false;
}
