#include <ScopeTimer.hpp>
Seer::ScopeTimer::ScopeTimer(const std::string name) :
	_name(name),
	_creation(std::chrono::steady_clock::now())
{	
	//Send start time
	Pipe::instance().send(
		std::make_unique<DataPoint::TimePoint>(_name, 0, _creation)
	);
}

Seer::ScopeTimer::~ScopeTimer()
{
	_destruction = std::chrono::steady_clock::now();
	//send end time to network
	Pipe::instance().send(
		std::make_unique<DataPoint::TimePoint>(_name, 1, _destruction, true)
	);
}