# Seer
**Master**  
[![Master](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=master)](https://travis-ci.org/ThomasMonkman/Seer)

**Develop**  
[![Develop](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=develop)](https://travis-ci.org/ThomasMonkman/Seer)

A C++ Remote Performance and Logging Library

# Table of Contents
1. [Use](#use)
2. [3rd Parties Used](#3rd-parties-used)
3. [Understanding behind the scenes](./documentation/Internals.md)

## Use

### ScopeTimer
ScopeTimer will measure the time spent in a block/scope.
This measure 2 seconds.
```c++
{
    Seer::ScopeTimer("test");
    std::this_thread::sleep_for(2s);
}
```

### Timer
Timer will measure the time between each tick().
This measure 2 seconds.
```c++
{
    Seer::Timer timer("test");
    timer.tick();
    std::this_thread::sleep_for(2s);
    timer.tick();
}
```

## 3rd Parties Used

* [Websocketpp](https://github.com/zaphoyd/websocketpp)
* [Aiso](https://github.com/chriskohlhoff/asio) - License (Boost Software License)
* [Catch](https://github.com/philsquared/Catch) - License (Boost Software License)
* [Json](https://github.com/nlohmann/json) - License (Boost Software License)
