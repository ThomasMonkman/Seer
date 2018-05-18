
# Seer <a href="#"><img src="https://img.shields.io/badge/C++-11-blue.svg?style=flat-square"></a>

| Branch        | Status        |
| ------------- |:-------------:|
| **Master**    | [![Master](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=master)](https://travis-ci.org/ThomasMonkman/Seer)|
| **Develop**   | [![Develop](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=develop)](https://travis-ci.org/ThomasMonkman/Seer) |

A single header performance logger outputting to chrome tracing in C++11 for windows, linux and mac, readable using [chrome://tracing](chrome://tracing)

Example when loaded in [chrome://tracing](chrome://tracing)
![Image](/doc/full-showcase.jpg?raw=true)

[Example code](#example)

### Install:
Drop [Seer.hpp](https://github.com/ThomasMonkman/Seer/blob/master/Seer.hpp) in to your include path and you should be good to go.
### Functions:
- [ScopeTimer](#1)
- [Async](#2)
- [Counter](#3)
- [instant_event](#4)
- [mark](#5)
- [set_thread_name](#6)
- [set_process_name](#7)

### Getting output and memory usage:
- [output](#101)
- [buffer and memory usage](#102)
- [buffer filled behaviour](#103)

### Speed:
- [benchmarks](#201)

### Tests:
- [tests](#301)
<hr>

### Functions:
#### ScopeTimer: <a id="1"></a>
ScopeTimer will measure the time spent in a block/scope.
```c++
{
    seer::ScopeTimer("test"); // This will measure 2 seconds.
    std::this_thread::sleep_for(2s);
}
```
![Image](/doc/scopetimer.jpg?raw=true)

#### Async: <a id="2"></a>
Async allows you to draw visual connections between scopes timers, this shows up in chrome as arrows joining blocks.
This is handy for tracking events across threads or separated by time.

Across Threads:
```c++

	seer::Async async;	
	{
		const auto timer = async.create_timer("Step 1");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	std::async(std::launch::async, [async] {
		const auto timer2 = async.create_timer("Step 2");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}).get();
	
	{
		const auto timer3 = async.create_timer("Step 3");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
```
![Image](/doc/async.jpg?raw=true)


Across time:
```c++

	seer::Async async;	
	{
		const auto timer = async.create_timer("Step 1");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
		// some other work
	{
		const auto timer2 = async.create_timer("Step 2");
		std::this_thread::sleep_for(std::chrono::seconds(1));	
	}
		// some more work
	{
		const auto timer3 = async.create_timer("Step 3");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
```
![Image](/doc/async-same-thread.jpg?raw=true)

#### Counter: <a id="3"></a>
Counter can be used for tracking a value over time. 
> Note: multiple counters sharing the same name will appear in the same chrome trace.
```c++
{
    seer::Counter<int> counter("test", 1);
    std::this_thread::sleep_for(2s);
    counter.update(2);
}
```
![Image](/doc/counter.jpg?raw=true)

#### instant_event: <a id="4"></a>
Will place an instant event scoped to either process (default), thread, globally.
```c++
{
	seer::instant_event("test"); // process
	seer::instant_event("test", seer::InstantEventScope::thread); // process
	seer::instant_event("test", seer::InstantEventScope::global); // global
}
```
Thread:

![Image](/doc/instant-event-thread.jpg?raw=true)

Process:

![Image](/doc/instant-event-process.jpg?raw=true)

Global:

![Image](/doc/instant-event-global.jpg?raw=true)

#### mark: <a id="5"></a>
Will produce a mark.
```c++
{
	seer::mark("test");
}
```
![Image](/doc/mark.jpg?raw=true)

#### set_thread_name: <a id="6"></a>
Will name the current thread or optional with another threads id.
```c++
{
	seer::set_thread_name("render");
	seer::set_thread_name("worker", worker.id);
}
```

#### set_process_name: <a id="7"></a>
Will name the current process.
```c++
{
	seer::set_process_name("my amazing app"); // this can be called at any point in time
}
```
![Image](/doc/set_process_name.jpg?raw=true)

<hr>

### Getting output and memory usage:
#### output: <a id="101"></a>
[chrome://tracing](chrome://tracing) expects a json file, this can be fetched in several way.

Interface to this data can be found through `seer::buffer`
```c++
{
	seer::buffer.dump_to_file("my profile data.json"); //defaults to "profile.json"
	file << seer::buffer; //by stream
	std::string json = seer::buffer.str(); //by string
}
```
#### buffer and memory usage: <a id="102"></a>
Internally Seer allocates a buffer of memory to write to, this defaults to ~10mb. Monitoring and changes to this buffer can be found through `seer::buffer`
```c++
{
	// usage
	seer::buffer.usage().usage_in_bytes;
	seer::buffer.usage().total_in_bytes;
	seer::buffer.usage().percent_used;
	
	// resize
	seer::buffer.resize(1000000000); // resize to 1gb

	// clear
	seer::buffer.clear();
}
```

#### buffer filled behaviour: <a id="103"></a>
When the internal buffer fills up by default it resets and starts a fresh, chucking out its old data, however this can be customised
```c++
{	
	seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::reset;
	// the default, the buffer forgets its old data and starts again

	seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::expand;
	// the buffer will grow on its own with no upper limit

	seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::discard;
	// the buffer will discard all new events once it fills
}
```

### Speed:
#### benchmarks: <a id="201"></a>
While seer may be header only, these benchmarks use google benchmark and require building.

Mac and linux:
```
{
	git clone https://github.com/ThomasMonkman/Seer.git
	cd ./Seer
	mkdir build && cd build/
	cmake -DCMAKE_BUILD_TYPE=Release ../
	make
	benchmarks/seer_benchmark
}
```
Windows:

Simply open the repo in visual studio 17, and with its new cmake features and you should just be able to run the projects, make sure you use release.

### Test:
#### test: <a id="301"></a>
While seer may be header only, these tests use catch 2 and require building.

Mac and linux:
```c++
{
	git clone https://github.com/ThomasMonkman/Seer.git
	cd ./Seer
	mkdir build && cd build/
	cmake -DCMAKE_BUILD_TYPE=Release ../
	make
	tests/seer_unit
}
```
Windows:

Simply open the repo in visual studio 17, and with its new cmake features and you should just be able to run the projects.

### Example code: <a id="example"></a>
```c++
{
{
	seer::set_process_name("my amazing app");
	seer::set_thread_name("render");
	seer::instant_event("frame start", seer::InstantEventScope::process);
	seer::Counter<int> tempture("tempture", 0);
	seer::Async async;
	seer::ScopeTimer frame("frame");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	tempture.update(2);
	{
		seer::ScopeTimer input("get input");
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		tempture.update(5);
		seer::instant_event("key press", seer::InstantEventScope::thread);
		test_helper::get_with_timeout<void>(std::async(std::launch::async, [async, &tempture] {
			seer::set_thread_name("input");
			const auto timer = async.create_timer("capture keypress");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			tempture.update(1);
		}));
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		tempture.update(10);
		const auto timer = async.create_timer("process keypress");
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		seer::mark("done");
	}
	tempture.update(4);
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
	seer::instant_event("sun outside", seer::InstantEventScope::global);

}
seer::buffer.dump_to_file("full-showcase.json");
}
```
