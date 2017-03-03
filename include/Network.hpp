#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "DataPoint.hpp"

#include "json\json.hpp"

// Set up websockets with asio and c++11 so I don't need boost
// https://github.com/zaphoyd/websocketpp
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_INTERNAL_
#include "websocketpp\config\core.hpp"
#include "websocketpp\config\asio_no_tls.hpp"
#include "websocketpp\server.hpp"

#include <memory> //std::unique_ptr
#include <vector> //std::vector
#include <thread> //std::thread, std::this_thread::sleep_until
#include <mutex> //std::mutex, std::lock_guard
#include <utility> //std::move
#include <future> //std::future
#include <exception> //std::exception_ptr, std::current_exception()
#include <algorithm> //std::remove_if
#include <chrono> //std::chrono_literals, std::chrono::steady_clock::now()
#include <atomic> //std::atomic
#include <ratio> //std::ratio

namespace Seer {
	//network heartbeat at 16.66ms
	using network_heartbeat = std::chrono::duration<float, std::ratio<1, 60>>;
	typedef websocketpp::server<websocketpp::config::asio> websocket_server;
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
			start_server();	
			_hearbeat = std::thread([this]() { heartbeat(); });
		}
		~Network();

		void heartbeat();
		void start_server();

		bool task_complete(std::future<void>& task);
		void consume_exception(std::exception_ptr&& exception);

		std::thread _hearbeat;

		websocket_server _server;
		
		std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> _data_points;
		std::vector<std::future<void>> _tasks;
		std::vector<std::exception_ptr> _exceptions_caught;
		std::mutex _data_point_mutex;
		std::mutex _exception_mutex;
		std::atomic<bool> _exception_has_been_raised = { false };
		std::atomic<bool> _destory = { false };
	};
}
#endif