#include <Timer.hpp>

Seer::Timer::Timer(const std::string name) : 
	_name(name), 
	_last_tick(std::chrono::steady_clock::now()) // In case tick is not called, we need a default to send out
{
}
Seer::Timer::~Timer()
{
	//close this timer, and just re emit the last point sent
	Pipe::instance().send(
		std::make_unique<DataPoint::TimePoint>(_name, _position, _last_tick, true)
	);
}
void Seer::Timer::tick()
{
	_last_tick = std::chrono::steady_clock::now();
	Pipe::instance().send(
		std::make_unique<DataPoint::TimePoint>(_name, _position++, _last_tick)
	);
}