#ifndef SCOPETIMER_HPP
#define SCOPETIMER_HPP

#include <chrono>
#include <string>
namespace Seer {
	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string name);
		~ScopeTimer();

	private:
		std::chrono::steady_clock::time_point _creation;
		std::chrono::steady_clock::time_point _destruction;
		const std::string _name;
	};
}
#endif