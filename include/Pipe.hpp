#ifndef PIPE_HPP
#define PIPE_HPP
// Seer
#include "DataPoint.hpp"
#include "Sink.hpp"
//#include "json\json.hpp"

#include <memory> //std::unique_ptr
#include <vector> //std::vector
#include <thread> //std::thread, std::this_thread::sleep_until
#include <mutex> //std::mutex, std::lock_guard
#include <atomic> //std::atomic
#include <algorithm> //std::any_of
#include <utility> //std::move

namespace Seer {
	//network heartbeat at 16.66ms
	using pipe_heartbeat = std::chrono::duration<float, std::ratio<1, 60>>;
	class Pipe
	{
	public:
		static Pipe& instance()
		{
			static Pipe pipe;
			return pipe;
		}
		//Send data
		void send(std::unique_ptr<DataPoint::BaseDataPoint> time_point);
	protected:
		void Seer::Pipe::add_sink(std::unique_ptr<Sink> sink);
	private:
		Pipe() 
		{
			_hearbeat = std::thread([this]() { heartbeat(); });			
		}
		~Pipe();

		void heartbeat();

		// places to put the DataPoints
		std::vector<std::unique_ptr<Sink>> _sinks;
		std::mutex _sink_mutex;

		std::vector<std::unique_ptr<DataPoint::BaseDataPoint>> _data_points;
		std::mutex _data_point_mutex;

		std::thread _hearbeat;

		std::atomic<bool> _destory = { false };
	};
}
#endif