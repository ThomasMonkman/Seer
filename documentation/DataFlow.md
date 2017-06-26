# Data Flow  

The data flow between C++ and web is in json, the below defines how these messages should be constructed for the various parts of the libary.  

I've shortened the names in the json keys to keep bandwidth as low as possible, I've put the full names in comments.

---
## C++ to Web

1. [Timers](#timers)
	1.1. [Scope Timer](#scope-timer)
	1.2. [Timer](#timer)

## Timers

### Scope Timer
```javascript
{
	"#": "tp", //Type - specifies this data as the timer
	"n": "", //Name - of the timer
	"t_id": "", //Thread_id, to islote this timer to the correct thread, also allows prediction of call stack
	"p" [0...], //Position - position in the sequence of this time point, belonging to this series.
	"t": "" //Time Point - the time point.
	"l": true|false, //Last - is this the last datapoint under this name, in this series.
}
```

### Timer
```javascript
{
	"#": "tp", //Type - specifies this data as the timer
	"n": "", //Name - of the timer
	"t_id": "", //Thread_id, to islote this timer to the correct thread, also allows prediction of call stack
	"p" [0...], //Position - position in the sequence of this time point, belonging to this series.
	"t": "" //Time Point - the time point.
	"l": true|false, //Last - is this the last datapoint under this name, in this series.
}
```

## Web to C++
