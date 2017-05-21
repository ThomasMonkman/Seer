#ifndef PIPE_HPP
#define PIPE_HPP
// Seer
#include "DataPoint.hpp"
#include "Sink.hpp"
#include "WebSocket.hpp"

#include "json\json.hpp"

#include <memory> //std::unique_ptr, std::make_unique, std::weak_ptr, std::make_shared
#include <vector> //std::vector
#include <thread> //std::thread, std::this_thread::sleep_until
#include <mutex> //std::mutex, std::lock_guard
#include <atomic> //std::atomic
#include <algorithm> //std::any_of
#include <utility> //std::move, std::size_t

namespace Seer {
	class BaseSink;

	//network heartbeat at 16.66ms
	using pipe_heartbeat = std::chrono::duration<float, std::ratio<1, 60>>;
	class Pipe
	{
		template<class CustomSink>
		friend class Sink;

	public:
		static Pipe& instance()
		{
			static Pipe pipe;
			return pipe;
		}
		//Send data
		void send(std::unique_ptr<DataPoint::BaseDataPoint> time_point);
		std::size_t number_of_sinks_attached();
	protected:
		std::weak_ptr<Seer::BaseSink> Seer::Pipe::add_sink(std::shared_ptr<Seer::BaseSink> sink);
		void Seer::Pipe::remove_sink(std::weak_ptr<Seer::BaseSink> sink);
	private:
		Pipe();
		~Pipe();

		void heartbeat();
				
		std::map<std::weak_ptr<Seer::BaseSink>,
			std::shared_ptr<Seer::BaseSink>,
			std::owner_less<std::weak_ptr<Seer::BaseSink>>> _sinks;

		std::mutex _sink_mutex;

		std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> _data_points;
		std::mutex _data_point_mutex;

		std::thread _hearbeat;

		std::atomic<bool> _destory = { false };
	};
}
#endif