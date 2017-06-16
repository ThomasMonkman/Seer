#include <Timer.hpp>

Seer::Timer::Timer(const std::string name) : _name(name)
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