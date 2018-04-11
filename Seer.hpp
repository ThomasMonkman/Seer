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
					switch (buffer_overflow_behaviour)
					{
					case BufferOverflowBehaviour::reset: 
						Pipe::i().clear();
						clear();
						break;
					case BufferOverflowBehaviour::expand: _store.resize(_store.size() * 1.5); break;
					case BufferOverflowBehaviour::exception: throw std::overflow_error("string store full");
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
				_store.clear();
				_head = 0;
			}

			std::size_t buffer_size() {
				return _store.size();
			}
		private:
			std::size_t _size { 1000 }; 
			std::vector<char> _store;
			std::mutex _mutex;
			std::size_t _head { 0 };

			StringStore() {
				_store.resize(_size);
			}
			~StringStore() = default;
			StringStore(const StringStore&) = delete;
			StringStore& operator=(const StringStore& other) = delete;
			StringStore(StringStore&&) = delete;
			StringStore& operator=(StringStore&& other) = delete;
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
			StringLookup thread_name;
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

		inline std::ostream& operator<<(std::ostream& out_stream, const DataPoint& event)
		{
			out_stream << "{\"name\":\"" << StringStore::i().get_from_store(event.name) //event.name
				<< "\",\"ph\":\"" << static_cast<char>(event.event_type)
				<< "\",\"pid\":" << 0
				<< ",\"tid\":" << event.thread_id
				<< ",\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(event.time_point.time_since_epoch()).count();
			// Add in optional extra depending on the data type
			switch (event.event_type)
			{
			case EventType::complete:
				out_stream << ",\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(event.extra.end_time - event.time_point).count();
			default:
				break;
			}

			out_stream << "}";
			return out_stream;
		}

		class Pipe
		{
		public:
			static Pipe& i() {
				static Pipe pipe;
				return pipe;
			}
			void send(DataPoint&& event) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				if (_events.size() >= _events.capacity()) {
					_events.clear();
				}
				_events.emplace_back(event);
			}
			/*void send(std::unique_ptr<event::BaseEvent> event) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				if (_events.size() < 10000000) {
					_events.clear();
				}
				_events.push_back(std::move(event));
			}*/
			void write_to_stream(std::ostream& stream) {
				std::lock_guard<std::mutex> lock(_event_mutex);
				//std::stringstream json;
				auto separator = '[';
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
		private:
			std::size_t _buffer_size_in_bytes = 10000000;
			std::vector<DataPoint> _events;
			std::mutex _event_mutex;

			Pipe() {
				_events.reserve(_buffer_size_in_bytes / sizeof(DataPoint));
			}			
			~Pipe() = default;
			Pipe(const Pipe&) = delete;
			Pipe& operator=(const Pipe& other) = delete;
			Pipe(Pipe&&) = delete;
			Pipe& operator=(Pipe&& other) = delete;
		};

		// Allows internal buffer to be streamed by just calling "<< seer::buffer"
		struct Buffer
		{
			std::string str() {
				std::stringstream ss;
				Pipe::i().write_to_stream(ss);
				return ss.str();
			}
			std::size_t size_in_bytes()
			{
				return internal::StringStore::i().buffer_size() + internal::Pipe::i().buffer_size();
			}
		};

		inline std::ostream& operator<<(std::ostream& out_stream, const Buffer& buffer)
		{
			Pipe::i().write_to_stream(out_stream);
			return out_stream;
		}
	}

	static internal::Buffer buffer;

	void dump_to_file(const std::string& file_name = "profile.json") {
		std::ofstream file(file_name);
		internal::Pipe::i().write_to_stream(file);
		file << std::flush;
	}

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
			internal::Pipe::i().send({
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
