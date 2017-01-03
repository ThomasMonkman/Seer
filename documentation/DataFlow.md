# Data Flow
The data flow between C++ and web is in json, the below defines how these messages should be constructed for the various parts of the libary.
I've shortened the names in the json keys to keep bandwidth as low as possible, I've put the full names in comments
---
## C++ to Web

## Timers

### Scope Timer
```javascript
{
	"#": "tp", //Type - specifies this data as the timer
	"n": "", //Name - of the timer
	"t_id": "", //Thread_id, to islote this timer to the correct thread, also allows prediction of call stack
	"s" true|false, //Start - if this is the start of the timer, or the end.
	"t": "" //Time Point - the time point
}
```
### Timer


### Scope Timer

### Scope Timer

### Scope Timer


## Web to C++