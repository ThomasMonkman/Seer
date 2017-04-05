#ifndef SINK_HPP
#define SINK_HPP
// Seer
// std
#include <atomic> //std::atomic
#include <string> //std::string
namespace Seer {
	//network heartbeat at 16.66ms
	class Sink
	{
	public:
		Sink()
		{
		}
		virtual ~Sink()
		{
		}

		virtual const bool active() const
		{
			bool active = _active;
			return active;
		}
		//Send data
		virtual void send(const std::string& data)
		{}
	private:
		std::atomic<bool> _active = { false };
	};
}
#endif