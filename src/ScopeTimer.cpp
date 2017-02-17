#include <ScopeTimer.hpp>
Seer::ScopeTimer::ScopeTimer(const std::string name) :
	_name(name),
	_creation(std::chrono::steady_clock::now())
{
	//Send start time
}

Seer::ScopeTimer::~ScopeTimer()
{
	_destruction = std::chrono::steady_clock::now();
	//send end time	
}