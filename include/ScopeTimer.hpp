#ifndef SCOPETIMER_HPP
#define SCOPETIMER_HPP

#include "DataPoint.hpp"
#include "Pipe.hpp"

#include <chrono>
#include <string>
#include <thread>
namespace Seer {
	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string name);
		~ScopeTimer();
	private:
		const std::chrono::steady_clock::time_point _creation;
		std::chrono::steady_clock::time_point _destruction;
		const std::string _name;
	};
}
#endif