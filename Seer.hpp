// MIT License
// 
// Copyright(c) 2016 Thomas Monkman
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SEER_HPP
#define SEER_HPP
// outputs chrome tracer json format https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview#
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <mutex>
#include <exception>
#include <algorithm>
#include <functional>
#include <sstream>
#include <exception>
#include <atomic>
#include <set>

#if defined(__linux__) || defined(__APPLE__)
#	include <sys/types.h>
#	include <unistd.h>
#elif _WIN32
#	include "Windows.h"
#endif

/*!
@brief namespace for Seer
@see https://github.com/ThomasMonkman/Seer
*/
namespace seer {
	/*!
	@brief Behaviour of buffers when filled.

	reset - the default, the buffer forgets its old data and starts again
	expand - the buffer will grow on its own with no upper limit
	discard - the buffer will discard all new events once it fills

	*/
	enum class BufferOverflowBehaviour {
		reset,
		expand,
		discard
	};
	static BufferOverflowBehaviour buffer_overflow_behaviour = BufferOverflowBehaviour::reset;
	namespace internal {
		struct StringLookup
		{
			std::uint32_t pos;
			std::uint32_t length;
		};

		class Pipe;

		class StringStore
		{
		public:
			static StringStore& i() {
				static StringStore pipe;
				return pipe;
			}
			StringLookup store(const std::string& string_to_store) {
				std::lock_guard<std::mutex> lock(_mutex);
				if (string_to_store.size() + _head > _store.size())
				{
					if (string_to_store.size() > _store.size() && buffer_overflow_behaviour == BufferOverflowBehaviour::reset) {
						return { 0, 0 };
					}
					switch (buffer_overflow_behaviour)
					{
					case BufferOverflowBehaviour::reset:
						_head = 0;
						_clear_callback();
						break;
					case BufferOverflowBehaviour::expand: _store.resize(static_cast<std::size_t>(_store.size() * 1.5f)); break;
					case BufferOverflowBehaviour::discard: return { 0, 0 };
					}
				}
				const auto insert_position = _store.begin() + _head;
				_head = std::distance(_store.begin(), _store.begin() + _head) + string_to_store.size();
				std::copy(string_to_store.begin(), string_to_store.end(), insert_position);

				return {
					static_cast<std::uint32_t>(std::distance(_store.begin(), insert_position)),
					static_cast<std::uint32_t>(string_to_store.size())
				};
			}
			std::string get_from_store(const StringLookup& lookup) {
				return {
					_store.begin() + lookup.pos,
					_store.begin() + lookup.pos + lookup.length,
				};
			}

			void clear() {
				std::lock_guard<std::mutex> lock(_mutex);
				_head = 0;
			}

			std::size_t buffer_size() {
				std::lock_guard<std::mutex> lock(_mutex);
				return _store.size();
			}

			std::size_t buffer_used() {
				std::lock_guard<std::mutex> lock(_mutex);
				return _head;
			}

			void set_buffer_size(std::size_t size_in_bytes) {
				std::lock_guard<std::mutex> lock(_mutex);
				_store.resize(size_in_bytes);
				_head = 0;
				_clear_callback();
			}

			void set_clear_callback(std::function<void()> callback) {
				std::lock_guard<std::mutex> lock(_mutex);
				_clear_callback = callback;
			}
		private:
			std::size_t _size{ 5000000 };
			std::vector<char> _store;
			std::mutex _mutex;
			std::size_t _head{ 0 };
			std::function<void()> _clear_callback;

			StringStore() {
				_store.resize(_size);
			}
			~StringStore() = default;
			StringStore(const StringStore&) = delete;
			StringStore& operator=(const StringStore&) = delete;
			StringStore(StringStore&&) = delete;
			StringStore& operator=(StringStore&&) = delete;
		};

		enum class EventType : char {
			duration_begin = 'B',
			duration_end = 'E',
			complete = 'X',
			instant = 'i',
			counter = 'C',
			meta = 'M',
			mark = 'R',
			flow_start = 's',
			flow_step = 't'
		};

		enum class InstantEventScope : char {
			thread = 't',
			process = 'p',
			global = 'g'
		};

		union EventExtra {
			void* not_used;
			InstantEventScope instant;
			StringLookup counter_value;
			StringLookup meta_object;
			std::chrono::steady_clock::time_point end_time;
			std::size_t flow_id;
		};

		struct Event
		{
			// common to everything
			StringLookup name;
			EventType event_type;
			std::thread::id thread_id;
			const std::chrono::steady_clock::time_point time_point;
			// extra
			EventExtra extra;
		};

		inline std::ostream& operator<<(std::ostream& out_stream, const StringLookup& string_store)
		{
			out_stream << StringStore::i().get_from_store(string_store);
			return out_stream;
		}

		inline std::ostream& operator<<(std::ostream& out_stream, const Event& event)
		{
			out_stream << "{\"name\":\"" << event.name
				<< "\",\"ph\":\"" << static_cast<char>(event.event_type)
#if defined(__linux__) || defined(__APPLE__)
				<< "\",\"pid\":" << ::getpid()
#elif _WIN32
				<< "\",\"pid\":" << ::GetCurrentProcessId()
#else
				<< "\",\"pid\":" << 0
#endif
				<< ",\"tid\":\"" << event.thread_id
				<< "\",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(event.time_point.time_since_epoch()).count();
			// Add in optional extra depending on the event type
			switch (event.event_type)
			{
			case EventType::complete:
				out_stream << ",\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(event.extra.end_time - event.time_point).count(); break;
			case EventType::instant:
				out_stream << ",\"s\":\"" << static_cast<char>(event.extra.instant) << "\""; break;
			case EventType::counter:
				out_stream << ",\"args\":{\"" << event.name << "\":" << event.extra.counter_value << "}"; break;
			case EventType::meta:
				out_stream << ",\"args\":" << event.extra.meta_object; break;
			case EventType::flow_start:
				out_stream << ",\"id\":\"" << event.extra.flow_id << "\""; break;
			case EventType::flow_step:
				out_stream << ",\"id\":\"" << event.extra.flow_id << "\""; break;
			default:
				break;
			}

			out_stream << "}";
			return out_stream;
		}

		class EventStore
		{
		public:
			static EventStore& i() {
				static EventStore pipe;
				return pipe;
			}
			void send(Event&& event) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				if (_events.size() >= _events.capacity()) {
					switch (buffer_overflow_behaviour)
					{
					case BufferOverflowBehaviour::reset:
						_events.clear();
						_clear_callback();
						break;
					case BufferOverflowBehaviour::expand: break; //let the vector use its own growth functions
					case BufferOverflowBehaviour::discard: return;
					}
				}
				_events.push_back(event);
			}

			void write_to_stream(std::ostream& stream) {
				std::lock_guard<std::mutex> lock(_event_mutex);

				//find all flow events and set the starting one as a flow start
				std::set<std::size_t> flow_events_found;
				for (const auto& event : _events) {
					if (event.event_type == EventType::flow_step && flow_events_found.find(event.extra.flow_id) == flow_events_found.end()) {
						flow_events_found.insert(event.extra.flow_id);
						// check we have not already once converted this flow event another this buffer was streamed
						const auto found = std::find_if(_events.begin(), _events.end(), [&event](const Event& e) { 
							return e.event_type == EventType::flow_start && e.extra.flow_id == event.extra.flow_id;
						});
						if (found == _events.end()) {
							// change the first flow event for the id, to a start flow
							const auto first = std::find_if(_events.begin(), _events.end(), [&event](const Event& e) {
								return e.event_type == EventType::flow_step && e.extra.flow_id == event.extra.flow_id;
							});

							auto start_flow_event = std::min_element(first, _events.end(), [&event](const Event& a, const Event& b) {
								if (a.event_type != EventType::flow_step || b.event_type != EventType::flow_step ||
									a.extra.flow_id != event.extra.flow_id || b.extra.flow_id != event.extra.flow_id) {
									return false;
								}
								return a.time_point < b.time_point;
							});
							start_flow_event->event_type = EventType::flow_start;
						}
					}
				}
								
				auto separator = '[';
				if (_events.size() == 0) {
					stream << separator;
				}
				for (const auto& event : _events)
				{
					stream << separator << event;
					separator = ',';
				}
				stream << ']';
			}

			void clear() {
				std::lock_guard<std::mutex> lock(_event_mutex);
				_events.clear();
			}

			std::size_t buffer_size() {
				std::lock_guard<std::mutex> lock(_event_mutex);
				return _events.capacity() * sizeof(Event);
			}

			std::size_t buffer_used() {
				std::lock_guard<std::mutex> lock(_event_mutex);
				return _events.size() * sizeof(Event);
			}

			void set_buffer_size(std::size_t size_in_bytes) {
				if (size_in_bytes < sizeof(Event) && buffer_overflow_behaviour != BufferOverflowBehaviour::expand) {
					throw std::length_error("buffer too small to fit even a single event");
				}
				std::lock_guard<std::mutex> lock(_event_mutex);
				_events.clear();
				_events.shrink_to_fit();
				_events.reserve(size_in_bytes / sizeof(Event));
				_clear_callback();
			}

			void set_clear_callback(std::function<void()> callback) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				_clear_callback = callback;
			}
		private:
			std::size_t _buffer_size_in_bytes = 5000000;
			std::vector<Event> _events;
			std::mutex _event_mutex;
			std::function<void()> _clear_callback;

			EventStore() {
				_events.reserve(_buffer_size_in_bytes / sizeof(Event));
			}
			~EventStore() = default;
			EventStore(const EventStore&) = delete;
			EventStore& operator=(const EventStore&) = delete;
			EventStore(EventStore&&) = delete;
			EventStore& operator=(EventStore&&) = delete;
		};

		struct BufferStats {
			const std::size_t usage_in_bytes;
			const std::size_t total_in_bytes;
			const double percent_used;
		};

		/*!
		@brief Allows access to seers buffer for serialisation and modification.

		file << seer::buffer; //by stream
		seer::buffer.dump_to_file("my profile data.json"); //defaults to "profile.json"
		std::string json = seer::buffer.str(); //by string

		*/
		struct Buffer
		{
			std::string str() {
				std::stringstream ss;
				EventStore::i().write_to_stream(ss);
				return ss.str();
			}

			BufferStats usage() const {
				const auto usage_in_bytes = internal::StringStore::i().buffer_used() + internal::EventStore::i().buffer_used();
				const auto total_in_bytes = internal::StringStore::i().buffer_size() + internal::EventStore::i().buffer_size();
				return {
					usage_in_bytes,
					total_in_bytes,
					static_cast<double>(usage_in_bytes / total_in_bytes)
				};
			}

			void dump_to_file(const std::string& file_name = "profile.json") {
				std::ofstream file(file_name);
				internal::EventStore::i().write_to_stream(file);
				file << std::flush;
			}

			void resize(std::size_t size_in_bytes) {
				internal::StringStore::i().set_buffer_size(size_in_bytes / 2);
				internal::EventStore::i().set_buffer_size(size_in_bytes / 2);
			}

			void clear() {
				EventStore::i().clear();
				StringStore::i().clear();
			}
		};

		inline std::ostream& operator<<(std::ostream& out_stream, const Buffer&)
		{
			EventStore::i().write_to_stream(out_stream);
			return out_stream;
		}

		class Coordinator {
		public:
			Coordinator() {
				StringStore::i().set_clear_callback([] { EventStore::i().clear(); });
				EventStore::i().set_clear_callback([] { StringStore::i().clear(); });
			}
			~Coordinator() = default;
			Coordinator(const Coordinator&) = delete;
			Coordinator& operator=(const Coordinator&) = delete;
			Coordinator(Coordinator&&) = delete;
			Coordinator& operator=(Coordinator&&) = delete;

			std::atomic<std::size_t> async_id{0};
		};

		static Coordinator coordinator;
	}

	static internal::Buffer buffer;

	/*!
	@brief Times lifetime of its self, there by timing the current scope block.
	*/
	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string& name) :
			_creation(std::chrono::steady_clock::now()),
			_name(internal::StringStore::i().store(name))
		{}
		~ScopeTimer() {

			//send end time to network
			internal::EventExtra extra = { nullptr };
			extra.end_time = std::chrono::steady_clock::now();
			internal::EventStore::i().send({
				_name,
				internal::EventType::complete,
				std::this_thread::get_id(),
				_creation,
				extra
				});
		}
		ScopeTimer(const ScopeTimer&) = delete;
		ScopeTimer& operator=(const ScopeTimer& other) = delete;
		ScopeTimer(ScopeTimer&&) = delete;
		ScopeTimer& operator=(ScopeTimer&& other) = delete;
	private:
		const std::chrono::steady_clock::time_point _creation;
		const internal::StringLookup _name;
	};

	using InstantEventScope = internal::InstantEventScope;

	/*!
	@brief Puts a scopable event at current time, allowing marking of key events across threads.
	*/
	static void instant_event(const std::string& name, const InstantEventScope event_type = InstantEventScope::process) {
		internal::EventExtra extra = { nullptr };
		extra.instant = event_type;
		internal::EventStore::i().send({
			internal::StringStore::i().store(name),
			internal::EventType::instant,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}

	/*!
	@brief Stores values over time, numbers, boolean, etc...
	*/
	template<typename T>
	class Counter
	{
	public:
		Counter(const std::string& name, const T& value) :
			_name(internal::StringStore::i().store(name))
		{
			store(stringify<T>(value));
		}
		~Counter() {}
		void update(const T& value) {
			store(stringify<T>(value));
		}
	private:
		const internal::StringLookup _name;


		Counter(const Counter&) = delete;
		Counter& operator=(const Counter&) = delete;
		Counter(Counter&&) = delete;
		Counter& operator=(Counter&&) = delete;

		template<
			class Q = T,
			typename = std::enable_if<std::is_arithmetic<Q>::value>::type
		>		
		std::string	stringify(const Q value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str();
		}

		// anything else should be a string, so we add quotes
		template<
			class Q = T,
			typename = std::enable_if<!std::is_arithmetic<Q>::value>::type
		>
		std::string stringify(const Q& value)
		{
			std::stringstream ss;
			ss << "\"" << value << "\"";
			return ss.str();
		}

		// boolean should be "true"|"false"
		template<class Q = T, typename = void>
		std::string stringify(const bool value)
		{
			std::stringstream ss;
			ss << std::boolalpha << value;
			return ss.str();
		}

		void store(const std::string& value) {
			internal::EventExtra extra = { nullptr };
			extra.counter_value = internal::StringStore::i().store(value);
			internal::EventStore::i().send({
				_name,
				internal::EventType::counter,
				std::this_thread::get_id(),
				std::chrono::steady_clock::now(),
				extra
				});
		}
	};

	/*!
	@brief Sets friendly thread name.
	*/
	static void set_thread_name(const std::string& name, std::thread::id thread_id = std::this_thread::get_id()) {
		internal::EventExtra extra = { nullptr };
		extra.meta_object = internal::StringStore::i().store("{\"name\":\"" + name + "\"}");
		internal::EventStore::i().send({
			internal::StringStore::i().store("thread_name"),
			internal::EventType::meta,
			thread_id,
			std::chrono::steady_clock::now(),
			extra
			});
	}

	/*!
	@brief Sets friendly process name.
	*/
	static void set_process_name(const std::string& name) {
		internal::EventExtra extra = { nullptr };
		extra.meta_object = internal::StringStore::i().store("{\"name\":\"" + name + "\"}");
		internal::EventStore::i().send({
			internal::StringStore::i().store("process_name"),
			internal::EventType::meta,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}

	/*!
	@brief Puts a very small instant marker at current time.
	*/
	static void mark(const std::string& name) {
		internal::EventExtra extra = { nullptr };
		internal::EventStore::i().send({
			internal::StringStore::i().store(name),
			internal::EventType::mark,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}

	/*!
	@brief Allows linking timings across threads and time, e.g. a network request, download, and processing of data.
	*/
	class Async
	{
	public:
		class Timer
		{
		public:
			Timer(const std::string& name, std::size_t async_id) :
				_creation(std::chrono::steady_clock::now()),
				_name(internal::StringStore::i().store(name)),
				_async_id(async_id)
			{}
			~Timer() {
				const auto end_time = std::chrono::steady_clock::now();
				internal::EventExtra extra_flow = { nullptr };
				extra_flow.flow_id = _async_id;
				internal::EventStore::i().send({
					_name,
					internal::EventType::flow_step,
					std::this_thread::get_id(),
					_creation,
					extra_flow
				});

				internal::EventExtra extra = { nullptr };
				extra.end_time = end_time;
				internal::EventStore::i().send({
					_name,
					internal::EventType::complete,
					std::this_thread::get_id(),
					_creation,
					extra
				});
			}
			Timer(const Timer&) = delete;
			Timer& operator=(const Timer& other) = delete;
			Timer(Timer&&) = default;
			Timer& operator=(Timer&& other) = default;
		private:
			const std::chrono::steady_clock::time_point _creation;
			const internal::StringLookup _name;
			const std::size_t _async_id;
		};
		Async() :
			id(internal::coordinator.async_id.fetch_add(1))
		{}
		~Async() = default;

		Timer create_timer(const std::string& name) const {
			return { name, id };
		}

		Async(const Async&) = default;
		Async& operator=(const Async& other) = default;
		Async(Async&&) = default;
		Async& operator=(Async&& other) = default;
	private:	
		const std::size_t id;
	};
}
////Duration 
//{
//	"name": "",
//	"ph" : "B" | "E", // phase
//	"pid" : 0, // can be injected at the end
//	"tid" : 0, // phase
//	"ts" : 0, // mircoseconds
//}
//
////InstantEvent 
//{
//	"name": "",
//	"ph" : "i", // phase
//	"pid" : 0, // can be injected at the end
//	"tid" : 0, // phase
//	"ts" : 0, // mircoseconds
//		"s" : "t" | "p" | "g", // event type
//}
////Counter 
//{
//	"name": "",
//	"ph" : "C", // phase
//	"pid" : 0, // can be injected at the end
//	"tid" : 0, // phase
//	"ts" : 0, // mircoseconds
//	"args" : {
//		"<name>": "value", // event type // translate value immediately to string
//	}
//}
////Set thread name 
//{
//	"name": "thread_name",
//	"ph" : "M", // phase
//	"pid" : 0, // can be injected at the end
//	"tid" : 0, // phase
//	"args" : {
//		"name": "threadname", // event type
//	}
//}
////Mark
//{
//	"name": "thread_name",
//	"ph" : "R", // phase
//	"pid" : 0, // can be injected at the end
//	"tid" : 0, // phase
//	"ts" : 0, // mircoseconds
//}

#endif // SEER_HPP
