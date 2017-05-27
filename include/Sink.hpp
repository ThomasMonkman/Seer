#ifndef SINK_HPP
#define SINK_HPP
// Seer
#include "Pipe.hpp"
#include "BaseSink.hpp"
// std
#include <utility> //std::forward
#include <memory> //std::weak_ptr, std::make_shared
#include <type_traits> //std::is_base_of
namespace Seer {
	/// @brief Will contruct and register a custom sink to Seer
	/// @code
			// A custom sink has been made (FileSink), that will dump to file
			// This sink will auto register, and auto un-register when destructed
	///		auto file_sink = Sink<FileSink>("file.log");
	/// @endcode
	template<class CustomSink>
	class Sink
	{
		static_assert(std::is_base_of<BaseSink, CustomSink>::value, "CustomSink must derive from BaseSink");
		static_assert(std::is_convertible<CustomSink*, Seer::BaseSink*>::value, "Seer::BaseSink inheritance must be public");
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
		/// @brief Returns the created sink, this can be useful if you want access it while its active in the Pipe
		std::shared_ptr<CustomSink> get_sink() {
			return std::static_pointer_cast<CustomSink>(std::shared_ptr<BaseSink>(_sink));
		}
	private:
		std::weak_ptr<BaseSink> _sink;
	};
}
#endif