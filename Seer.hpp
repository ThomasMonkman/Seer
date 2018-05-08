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

#if defined(__linux__) || defined(__APPLE__)
#	include <sys/types.h>
#	include <unistd.h>
#elif _WIN32
#	include "Windows.h"
#endif

namespace seer {
	enum class BufferOverflowBehaviour {
		reset,
		expand,
		exception
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
					if (string_to_store.size() > _store.size()) {
						throw std::length_error("String store not big enough for string");
					}
					switch (buffer_overflow_behaviour)
					{
					case BufferOverflowBehaviour::reset:
						_head = 0;
						_clear_callback();
						break;
					case BufferOverflowBehaviour::expand: _store.resize(static_cast<std::size_t>(_store.size() * 1.5f)); break;
					case BufferOverflowBehaviour::exception: throw std::overflow_error("String store full");
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
				return _store.size();
			}

			void set_clear_callback(std::function<void()> callback) {
				_clear_callback = callback;
			}
		private:
			std::size_t _size{ 10000000 };
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
			mark = 'R'
		};

		enum class InstantEventScope : char {
			thread = 't',
			process = 'p',
			global = 'g'
		};

		union DataPointExtra {
			void* not_used;
			InstantEventScope instant;
			StringLookup counter_value;
			StringLookup meta_object;
			std::chrono::steady_clock::time_point end_time;
		};

		struct DataPoint // 40 bytes
		{
			// common to everything
			StringLookup name; //8 bytes
			EventType event_type; // 1 bytes
			std::thread::id thread_id; // 4 bytes
			const std::chrono::steady_clock::time_point time_point; // 4 bytes
																	// extra
			DataPointExtra extra; // 8 bytes;
		};

		inline std::ostream& operator<<(std::ostream& out_stream, const StringLookup& string_store)
		{
			out_stream << StringStore::i().get_from_store(string_store);
			return out_stream;
		}

		inline std::ostream& operator<<(std::ostream& out_stream, const DataPoint& event)
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
			// Add in optional extra depending on the data type
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
			void send(DataPoint&& event) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				if (_events.size() >= _events.capacity()) {
					switch (buffer_overflow_behaviour)
					{
					case BufferOverflowBehaviour::reset:
						_events.clear();
						_clear_callback();
						break;
					case BufferOverflowBehaviour::expand: break; //let the vector use its own growth functions
					case BufferOverflowBehaviour::exception: throw std::overflow_error("Event store full");
					}
				}
				_events.emplace_back(event);
			}

			void write_to_stream(std::ostream& stream) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				//std::stringstream json;
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
				return _events.capacity();
			}

			void set_clear_callback(std::function<void()> callback) {
				_clear_callback = callback;
			}
		private:
			std::size_t _buffer_size_in_bytes = 10000000;
			std::vector<DataPoint> _events;
			std::mutex _event_mutex;
			std::function<void()> _clear_callback;

			EventStore() {
				_events.reserve(_buffer_size_in_bytes / sizeof(DataPoint));
			}
			~EventStore() = default;
			EventStore(const EventStore&) = delete;
			EventStore& operator=(const EventStore&) = delete;
			EventStore(EventStore&&) = delete;
			EventStore& operator=(EventStore&&) = delete;
		};

		struct BufferStats {
			std::size_t usage_in_bytes;
			std::size_t total_in_bytes;
			
			/*std::size_t usage_in_bytes;
			std::size_t total_in_bytes;*/
			
			double percent_used() {
				return usage_in_bytes / total_in_bytes;
			}
		};

		// Allows internal buffer to be streamed by just calling "<< seer::buffer"
		struct Buffer
		{
			std::string str() {
				std::stringstream ss;
				EventStore::i().write_to_stream(ss);
				return ss.str();
			}

			BufferStats usage() {
				return {
					0,
					internal::StringStore::i().buffer_size() + internal::EventStore::i().buffer_size()
				};
			}

			void dump_to_file(const std::string& file_name = "profile.json") {
				std::ofstream file(file_name);
				internal::EventStore::i().write_to_stream(file);
				file << std::flush;
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
		};

		static Coordinator coordinator;
	}

	static internal::Buffer buffer;

	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string& name) :
			_creation(std::chrono::steady_clock::now()),
			_name(internal::StringStore::i().store(name))
		{}
		~ScopeTimer() {

			//send end time to network
			internal::DataPointExtra extra = { nullptr };
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

	static void instant_event(const std::string& name, const InstantEventScope event_type = InstantEventScope::thread) {
		internal::DataPointExtra extra = { nullptr };
		extra.instant = event_type;
		internal::EventStore::i().send({
			internal::StringStore::i().store(name),
			internal::EventType::instant,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}

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

		template<class Q = T>
		typename std::enable_if<std::is_arithmetic<Q>::value, std::string>::type
			stringify(const Q value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str();
		}

		// anything else should be a string, so we add quotes
		template<class Q = T>
		typename std::enable_if<!std::is_arithmetic<Q>::value, std::string>::type
			stringify(const Q& value)
		{
			std::stringstream ss;
			ss << "\"" << value << "\"";
			return ss.str();
		}

		// boolean should be "true"|"false"
		template<class Q = T>
		std::string
			stringify(const bool value)
		{
			std::stringstream ss;
			ss << std::boolalpha << value;
			return ss.str();
		}

		void store(const std::string& value) {
			internal::DataPointExtra extra = { nullptr };
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

	static void set_thread_name(const std::string& name, std::thread::id thread_id = std::this_thread::get_id()) {
		internal::DataPointExtra extra = { nullptr };
		extra.meta_object = internal::StringStore::i().store("{\"name\":\"" + name + "\"}");
		internal::EventStore::i().send({
			internal::StringStore::i().store("thread_name"),
			internal::EventType::meta,
			thread_id,
			std::chrono::steady_clock::now(),
			extra
			});
	}

	static void set_process_name(const std::string& name) {
		internal::DataPointExtra extra = { nullptr };
		extra.meta_object = internal::StringStore::i().store("{\"name\":\"" + name + "\"}");
		internal::EventStore::i().send({
			internal::StringStore::i().store("process_name"),
			internal::EventType::meta,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}

	static void mark(const std::string& name) {
		internal::DataPointExtra extra = { nullptr };
		internal::EventStore::i().send({
			internal::StringStore::i().store(name),
			internal::EventType::mark,
			std::this_thread::get_id(),
			std::chrono::steady_clock::now(),
			extra
			});
	}
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

//namespace seer {
//	enum class EventScope {
//		thread,
//		process,
//		global
//	};
//
//	inline std::ostream& operator<<(std::ostream& out, const EventScope& event)
//	{
//		switch (event)
//		{
//		case EventScope::thread:
//			out << 't';
//			break;
//		case EventScope::process:
//			out << 'p';
//			break;
//		case EventScope::global:
//			out << 'g';
//			break;
//		}
//		return out;
//	}
//	namespace internal {
//		namespace event {
//			struct BaseEvent
//			{
//				virtual ~BaseEvent() {}
//
//				/**
//				* \brief: output this DataPoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				virtual void print_data(std::ostream &out_stream) const
//				{
//					out_stream << "{}";
//				}
//			};
//			inline std::ostream& operator<<(std::ostream& out, const BaseEvent& base_data_point)
//			{
//				base_data_point.print_data(out);
//				return out;
//			}
//
//			struct DurationEvent : public BaseEvent
//			{
//				/**
//				* \brief Create a datapoint specialised around timers.
//				* \param name of the Event, the ending event must have the same name.
//				* \param time_point to store.
//				* \param start is this the first datapoint of the pair under this name, in this series.
//				*/
//				DurationEvent(const std::string name,
//					const std::chrono::steady_clock::time_point time_point,
//					const bool start) :
//					name(name),
//					thread_id(std::hash<std::thread::id>()(std::this_thread::get_id())),
//					time_point(time_point),
//					start(start)
//				{}
//
//				~DurationEvent() override
//				{
//				}
//
//				const std::string name;
//				const std::size_t thread_id;
//				const std::chrono::steady_clock::time_point time_point;
//				const bool start;
//
//				/**
//				* \brief: output this TimePoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				void print_data(std::ostream &out_stream) const override
//				{
//					out_stream << "{\"name\":\"" << name
//						<< "\",\"ph\":\"" << (start ? 'B' : 'E')
//						<< "\",\"pid\":" << 0
//						<< ",\"tid\":" << thread_id
//						<< ",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count()
//						<< "}";
//				};
//			};
//
//			inline std::ostream& operator<<(std::ostream& out, const DurationEvent& duration_event)
//			{
//				duration_event.print_data(out);
//				return out;
//			}
//
//			struct InstantEvent : public BaseEvent
//			{
//				/**
//				* \brief Create a datapoint specialised around timers.
//				* \param name of the Event, the ending event must have the same name.
//				* \param time_point to store.
//				* \param start is this the first datapoint of the pair under this name, in this series.
//				*/
//				InstantEvent(const std::string name,
//					const std::chrono::steady_clock::time_point time_point,
//					const seer::EventScope event) :
//					name(name),
//					thread_id(std::hash<std::thread::id>()(std::this_thread::get_id())),
//					time_point(time_point),
//					event(event)
//				{}
//
//				~InstantEvent() override
//				{
//				}
//
//				const std::string name;
//				const std::size_t thread_id;
//				const std::chrono::steady_clock::time_point time_point;
//				const EventScope event;
//				/**
//				* \brief: output this TimePoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				void print_data(std::ostream &out_stream) const override
//				{
//					out_stream << "{\"name\":\"" << name
//						<< "\",\"ph\":\"" << "i"
//						<< "\",\"pid\":" << 0
//						<< ",\"tid\":" << thread_id
//						<< ",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count()
//						<< ",\"s\":\"" << event
//						<< "\"}";
//				};
//			};
//
//			inline std::ostream& operator<<(std::ostream& out, const InstantEvent& instant_event)
//			{
//				instant_event.print_data(out);
//				return out;
//			}
//
//			template<typename T>
//			struct CounterEvent : public BaseEvent
//			{
//				/**
//				* \brief Create a datapoint specialised around timers.
//				* \param name of the Event, the ending event must have the same name.
//				* \param time_point to store.
//				* \param start is this the first datapoint of the pair under this name, in this series.
//				*/
//				CounterEvent(const std::string name,
//					const std::chrono::steady_clock::time_point time_point,
//					const T value) :
//					name(name),
//					thread_id(std::hash<std::thread::id>()(std::this_thread::get_id())),
//					time_point(time_point),
//					value(value)
//				{}
//
//				~CounterEvent() override
//				{
//				}
//
//				const std::string name;
//				const std::size_t thread_id;
//				const std::chrono::steady_clock::time_point time_point;
//				const T value;
//				/**
//				* \brief: output this TimePoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				void print_data(std::ostream &out_stream) const override
//				{
//					out_stream << "{\"name\":\"" << name
//						<< "\",\"ph\":\"" << "C"
//						<< "\",\"pid\":" << 0
//						<< ",\"tid\":" << thread_id
//						<< ",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count()
//						<< ",\"args\": { \"" << name << "\":" << value
//						<< "}}";
//				};
//			};
//
//			template<typename T>
//			inline std::ostream& operator<<(std::ostream& out, const CounterEvent<T>& counter_event)
//			{
//				counter_event.print_data(out);
//				return out;
//			}
//
//			struct ThreadEvent : public BaseEvent
//			{
//				/**
//				* \brief Create a datapoint specialised around timers.
//				* \param name of the Event, the ending event must have the same name.
//				* \param time_point to store.
//				* \param start is this the first datapoint of the pair under this name, in this series.
//				*/
//				ThreadEvent(const std::string name) :
//					name(name),
//					thread_id(std::hash<std::thread::id>()(std::this_thread::get_id()))
//				{}
//
//				~ThreadEvent() override
//				{
//				}
//
//				const std::string name;
//				const std::size_t thread_id;
//
//				/**
//				* \brief: output this TimePoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				void print_data(std::ostream &out_stream) const override
//				{
//					out_stream << "{\"name\":\"thread_name\",\"ph\":\"" << "M"
//						<< "\",\"pid\":" << 0
//						<< ",\"tid\":" << thread_id
//						<< ",\"args\": { \"name\":\"" << name
//						<< "\"}}";
//				};
//			};
//
//			inline std::ostream& operator<<(std::ostream& out, const ThreadEvent& thread_event)
//			{
//				thread_event.print_data(out);
//				return out;
//			}
//
//
//
//			struct MarkEvent : public BaseEvent
//			{
//				/**
//				* \brief Create a datapoint specialised around timers.
//				* \param name of the Event, the ending event must have the same name.
//				* \param time_point to store.
//				* \param start is this the first datapoint of the pair under this name, in this series.
//				*/
//				MarkEvent(const std::string name,
//					const std::chrono::steady_clock::time_point time_point) :
//					name(name),
//					thread_id(std::hash<std::thread::id>()(std::this_thread::get_id())),
//					time_point(time_point)
//				{}
//
//				~MarkEvent() override
//				{
//				}
//
//				const std::string name;
//				const std::size_t thread_id;
//				const std::chrono::steady_clock::time_point time_point;
//				/**
//				* \brief: output this TimePoint as json to the supplied ostream.
//				* \param out_stream to write to.
//				*/
//				void print_data(std::ostream &out_stream) const override
//				{
//					out_stream << "{\"name\":\"" << name
//						<< "\",\"ph\":\"" << "R"
//						<< "\",\"pid\":" << 0
//						<< ",\"tid\":" << thread_id
//						<< ",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()).count()
//						<< "}";
//				};
//			};
//
//			inline std::ostream& operator<<(std::ostream& out, const MarkEvent& mark_event)
//			{
//				mark_event.print_data(out);
//				return out;
//			}
//		}
//
//		class Pipe
//		{
//		public:
//			static Pipe& i() {
//				static Pipe pipe;
//				return pipe;
//			}
//			void send(std::unique_ptr<event::BaseEvent> event) {
//				std::lock_guard<std::mutex> lock(_event_mutex);
//				if (_events.size() < 10000000) {
//					_events.clear();
//				}
//				_events.push_back(std::move(event));
//			}
//			void dump_to_file(const std::string& file_name = "profile.json") {
//				std::lock_guard<std::mutex> lock(_event_mutex);
//				//std::stringstream json;
//				std::ofstream file(file_name);
//				auto separator = '[';
//				for (const auto& event : _events)
//				{
//					file << separator << *event;
//					separator = ',';
//				}
//				file << ']' << std::flush;
//			}
//		private:
//			std::vector<std::unique_ptr<event::BaseEvent>> _events;
//			std::mutex _event_mutex;
//
//			Pipe() {}
//			~Pipe() {}
//		};
//	}
//	class ScopeTimer
//	{
//	public:
//		ScopeTimer(const std::string name) :
//			_name(name),
//			_creation(std::chrono::steady_clock::now())
//		{
//#ifndef SEER_DISABLE
//			//Send start time
//			internal::Pipe::i().send(
//				std::make_unique<internal::event::DurationEvent>(_name, _creation, true)
//			);
//#endif // !SEER_DISABLE
//		}
//		~ScopeTimer() {
//#ifndef SEER_DISABLE
//			const auto _destruction = std::chrono::steady_clock::now();
//			//send end time to network
//			internal::Pipe::i().send(
//				std::make_unique<internal::event::DurationEvent>(_name, _destruction, false)
//			);
//#endif // !SEER_DISABLE
//		}
//	private:
//		const std::chrono::steady_clock::time_point _creation;
//		const std::string _name;
//	};
//
//	static void event(const std::string name, const EventScope event = EventScope::thread) {
//#ifndef SEER_DISABLE
//		internal::Pipe::i().send(
//			std::make_unique<internal::event::InstantEvent>(name, std::chrono::steady_clock::now(), event)
//		);
//#endif // !SEER_DISABLE
//	}
//
//	template<typename T>
//	struct Count
//	{
//		Count(const std::string name, const T value)
//			: _name(name)
//		{
//#ifndef SEER_DISABLE
//			internal::Pipe::i().send(
//				std::make_unique<internal::event::CounterEvent<T>>(name, std::chrono::steady_clock::now(), value)
//			);
//#endif // !SEER_DISABLE
//		}
//		~Count() {}
//		void update(const T value) {
//#ifndef SEER_DISABLE
//			internal::Pipe::i().send(
//				std::make_unique<internal::event::CounterEvent<T>>(_name, std::chrono::steady_clock::now(), value)
//			);
//#endif // !SEER_DISABLE
//		}
//	private:
//		const std::string _name;
//	};
//
//	// set current thread name
//	static void setThreadName(const std::string name) {
//#ifndef SEER_DISABLE
//		internal::Pipe::i().send(
//			std::make_unique<internal::event::ThreadEvent>(name)
//		);
//#endif // !SEER_DISABLE
//	}
//
//	static void marker(const std::string name) {
//#ifndef SEER_DISABLE
//		internal::Pipe::i().send(
//			std::make_unique<internal::event::MarkEvent>(name, std::chrono::steady_clock::now())
//		);
//#endif // !SEER_DISABLE
//	}
//}
//
#endif // SEER_HPP
