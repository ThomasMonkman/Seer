# Data Flow  

The data flow between C++ and web is in json, the below defines how these messages should be constructed for the various parts of the libary.  

I've shortened the names in the json keys to keep bandwidth as low as possible, I've put the full names in comments.

---
## C++ to Web

1. [Timers](#markdown-header-timers)
	1.1. [Scope Timer](#markdown-header-scope-timer)
	1.2. [Timer](#markdown-header-timer)

## Timers

### Scope Timer
```javascript
{
	"#": "tp", //Type - specifies this data as the timer
	"n": "", //Name - of the timer
	"t_id": "", //Thread_id, to islote this timer to the correct thread, also allows prediction of call stack
	"p" 0|1, //Position - 0 for start of the timer, 1 for the end.
	"t": "" //Time Point - the time point
}
```
### Timer

## Web to C++
