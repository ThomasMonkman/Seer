#include "ScopeTimer.hpp"
ScopeTimer::ScopeTimer(const std::string name) :
	_name(name),
	_creation(std::chrono::steady_clock::now())
{
	//Send start time
}

ScopeTimer::~ScopeTimer()
{
	_destruction = std::chrono::steady_clock::now();
	//send end time	
}