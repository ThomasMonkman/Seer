#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "DataPoint.hpp"

#include "json\json.hpp"

#include <memory> //std::unique_ptr
#include <vector> //std::vector
#include <thread> //std::thread
#include <mutex> //std::mutex, std::lock_guard
#include <utility> //std::move
#include <future> //std::future
#include <exception> //std::exception_ptr
#include <algorithm> //std::remove_if
#include <chrono> //std::chrono_literals
#include <atomic> //std::atomic
#include <sstream> //std::stringstream

namespace Seer {
	class Network
	{
	public:
		static Network& instance()
		{
			static Network network;
			return network;
		}
		void send(std::unique_ptr<DataPoint::BaseDataPoint> time_point);
	private:
		Network()
		{
			
		}
		~Network();

		void heartbeat();
		bool task_complete(std::future<void>& task);

		std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> _data_points;
		std::vector<std::future<void>> _tasks;
		std::vector<std::exception_ptr> _exceptions_caught;
		std::mutex _data_point_mutex;
		std::mutex _exception_mutex;
		std::atomic<bool> _exception_has_been_raised = { false };
	};
}
#endif