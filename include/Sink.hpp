#ifndef SINK_HPP
#define SINK_HPP
// Seer
#include "Pipe.hpp"
// std
#include <atomic> //std::atomic
#include <utility> //std::forward
#include <memory> //std::weak_ptr, std::make_shared
#include <string> //std::string
#include <type_traits> //std::is_base_of

class Pipe;

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

	/// @brief Will contruct and register a custom sink to Seer
	/// @code
			// A custom sink has been made (FileSink), that will dump to file
			// This sink will auto register, and auto un-register when destructed
	///		auto file_sink = Sink<FileSink>("file.log");
	/// @endcode
	class Pipe;

	template<class CustomSink>
	class Sink
	{
		static_assert(std::is_base_of<BaseSink, CustomSink>::value, "CustomSink must derive from BaseSink");
		//static_assert(std::is_convertible<CustomSink, Seer::BaseSink>::value, "Seer::BaseSink inheritance must be public");
	public:
		template<class... Args>
		Sink(Args&&... args)
		{
			_sink = Pipe::instance().add_sink(
				std::make_shared<CustomSink>(std::forward<Args>(args)...)
			);
		}
		~Sink()
		{
			Pipe::instance().remove_sink(_sink);
		}
		std::shared_ptr<CustomSink> get_sink() {
			return std::static_pointer_cast<CustomSink>(std::shared_ptr<BaseSink>(_sink));
		}
	private:
		std::weak_ptr<BaseSink> _sink;
	};
}
#endif