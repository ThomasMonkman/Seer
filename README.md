
# Seer <a href="#"><img src="https://img.shields.io/badge/C++-11-blue.svg?style=flat-square"></a>

| Branch        | Status        |
| ------------- |:-------------:|
| **Master**    | [![Master](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=master)](https://travis-ci.org/ThomasMonkman/Seer)|
| **Develop**   | [![Develop](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=develop)](https://travis-ci.org/ThomasMonkman/Seer) |

A single header performance logger outputting to chrome tracing in C++11 for windows and linux

### Install:
Drop [Seer.hpp](https://github.com/ThomasMonkman/Seer/blob/master/Seer.hpp) in to your include path and you should be good to go.
### Functions:
- [ScopeTimer](#1)
- [Counter](#2)
- [instant_event](#3)
- [mark](#4)
- [set_thread_name](#5)
- [set_process_name](#6)

### Getting output and memory usage:
- [output](#101)
- [buffer](#102)
- [memory usage](#103)

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

#### Counter: <a id="2"></a>
Counter can be used for tracking a value over time. 
> Note: multiple counters sharing the same name will appear in the same chrome trace.
```c++
{
    seer::Counter<int> counter("test", 1);
    std::this_thread::sleep_for(2s);
    counter.update(2);
}
```

#### instant_event: <a id="3"></a>
Will place an instant event scoped to either thread (default), process, globally.
```c++
{
	seer::instant_event("test"); // thread
	seer::instant_event("test", seer::InstantEventScope::process); // process
	seer::instant_event("test", seer::InstantEventScope::global); // global
}
```

#### mark: <a id="4"></a>
Will produce a mark.
```c++
{
	seer::mark("test");
}
```

#### set_thread_name: <a id="5"></a>
Will name the current thread or optional with another threads id.
```c++
{
	seer::set_thread_name("render");
	seer::set_thread_name("worker", worker.id);
}
```

#### set_process_name: <a id="6"></a>
Will name the current process.
```c++
{
	seer::set_process_name("my amazing app"); // this can be called at any point in time
}
```
<hr>

### Getting output and memory usage:
#### output: <a id="101"></a>
Chrome expects a json file, this can be fetched in several way.

Interface to this data can be found through `seer::buffer`
```c++
{
	seer::buffer.dump_to_file("my profile data.json"); //defaults to "profile.json"
	file << seer::buffer; //by stream
	std::string json = seer::buffer.str(); //by string
}
```
#### buffer: <a id="102"></a>
Internally Seer allocates a buffer of memory to write to,  this defaults to 50mb. Monitoring and changes to this buffer can be found through `seer::buffer`
```c++
{
	seer::buffer.size_in_bytes();
}
```
#### memory usage: <a id="103"></a>


### Speed:
#### benchmarks: <a id="201"></a>

### Test:
#### test: <a id="301"></a>
