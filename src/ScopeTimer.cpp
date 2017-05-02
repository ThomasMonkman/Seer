#include <ScopeTimer.hpp>
Seer::ScopeTimer::ScopeTimer(const std::string name) :
	_name(name),
	_creation(std::chrono::steady_clock::now())
{	
	//Send start time
	Pipe::instance().send(std::make_unique<DataPoint::TimePoint>
		(DataPoint::TimePoint(_name, std::this_thread::get_id(), true, _creation)));
	//DataPoint::TimePoint inside make_unique not needed!!!
}

Seer::ScopeTimer::~ScopeTimer()
{
	_destruction = std::chrono::steady_clock::now();
	//send end time to network
	Pipe::instance().send(std::make_unique<DataPoint::TimePoint>
		(DataPoint::TimePoint(_name, std::this_thread::get_id(), false, _destruction)));
}