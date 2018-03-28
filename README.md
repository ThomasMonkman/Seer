# Seer
**Master**  
[![Master](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=master)](https://travis-ci.org/ThomasMonkman/Seer)

**Develop**  
[![Develop](https://travis-ci.org/ThomasMonkman/Seer.svg?branch=develop)](https://travis-ci.org/ThomasMonkman/Seer)

A C++ Adapter for chrome tracing

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

* [Catch](https://github.com/philsquared/Catch) - License (Boost Software License)
