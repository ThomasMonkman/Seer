#ifndef SINK_HPP
#define SINK_HPP
// Seer
//#include "Pipe.hpp"
// std
#include <atomic> //std::atomic
#include <string> //std::string
#include <type_traits> //std::is_base_of

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

		virtual const bool active() const
		{
			bool active = _active;
			return active;
		}
		//Send data
		virtual void send(const std::string& data)
		{}
	protected:
		std::atomic<bool> _active = { false };
	private:
	};
	
	/// @brief Will contruct and register a custom sink to Seer
	/// @code
			// A custom sink has been made (FileSink), that will dump to file
			// This sink will auto register, and auto un-register when destructed
	///		auto file_sink = Sink<FileSink>("file.log");
	/// @endcode
	//Seer::Pipe;

	template<typename CustomSink, typename... Args>
	class Sink
	{
		static_assert(std::is_base_of<BaseSink, CustomSink>::value, "CustomSink must derive from BaseSink");
	public:
		Sink(Args&&... args)
		{
			/*Seer::Pipe::instance().add_sink(
				std::make_unique<CustomSink>(std::forward<Args>(args)...)
			);*/
		}
		~Sink() 
		{
			//Seer::Pipe::instance().remove_sink(sink_id);
		}

	private:
		std::size_t sink_id;
	};
}
#endif