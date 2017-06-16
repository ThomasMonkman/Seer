#ifndef TIMER_HPP
#define TIMER_HPP

#include "DataPoint/TimePoint.hpp"
#include "Pipe.hpp"

#include <chrono>
#include <string>
#include <thread>
namespace Seer {
	/**
	* \class ScopeTimer
	* \brief Create a scope based timer, which will logs its creation time and its destruction time.
	*/
	class Timer
	{
	public:
		Timer(const std::string name);
		~Timer();
		/**
		* \brief: Ticks this timer, sending the current time to the client, this will then be diffed against old times.
		*/
		void tick();
	private:
		const std::string _name;
		std::chrono::steady_clock::time_point _last_tick;
		std::size_t _position = { 0 };		
	};
}
#endif