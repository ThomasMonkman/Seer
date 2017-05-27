#ifndef BASESINK_HPP
#define BASESINK_HPP

#include "Pipe.hpp"

// std
#include <atomic> //std::atomic
#include <string> //std::string
namespace Seer {
	class BaseSink
	{
	public:
		BaseSink()
		{
		}
		virtual ~BaseSink()
		{
		}
		/// @brief Check for if this sink is active, inactive sinks will be skipped by the pipe, as such they will not receive data
		/// @returns whether this sink is active
		virtual const bool active() const
		{
			bool active = _active;
			return active;
		}
		/// @brief Will receive any message that comes through the pipe
		/// @param data Message received from pipe
		virtual void send(const std::string& data)
		{}
	protected:
		std::atomic<bool> _active = { true };
	private:
	};
}
#endif