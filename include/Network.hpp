#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "DataPoint.hpp"
#include "BaseProducer.hpp"
#include "BaseListener.hpp"

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
#include <queue> //std::queue
#include <string> //std::string
#include <set> //std::set
#include <map> //std::map
#include <condition_variable> //std::condition_variable

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
		void send(std::unique_ptr<Listener::BaseListener> listener);
	private:
		Network()
		{
			start_server();	
			_hearbeat = std::thread([this]() { heartbeat(); });
			_message_process_thread = std::thread([this]() { received_messages_worker(); });
		}
		~Network();

		void heartbeat();
		void start_server();
		void send_to_clients(const std::string& message);
		void received_messages_worker();
		void process_received_messages(const std::string& message);

		bool task_complete(std::future<void>& task);
		void consume_exception(std::exception_ptr&& exception);

		std::thread _hearbeat;
		std::thread _server_thread;
		std::thread _message_process_thread;
		websocket_server _server;
		std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> _connections;
		std::mutex _connection_mutex;

		std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> _data_points;
		std::mutex _data_point_mutex;

		std::queue<std::string> _received_messages;
		std::mutex _received_messages_mutex;
		std::condition_variable _received_messages_condition;

		std::map<std::string, std::unique_ptr<Listener::BaseListener>> _task_listeners;
		std::vector<std::future<void>> _running_tasks;
		std::vector<std::exception_ptr> _exceptions_caught;
		std::mutex _exception_mutex;
		std::atomic<bool> _exception_has_been_raised = { false };
		std::atomic<bool> _destory = { false };
	};
}
#endif