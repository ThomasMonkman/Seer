
#include "benchmark/benchmark.h"
#include "../Seer.hpp"

#include <thread>
#include <mutex>

static void scope_timer(benchmark::State& state) {
	if (state.thread_index == 0) {
		seer::buffer.clear();
	}
	for (auto _ : state) {
		seer::ScopeTimer("test");
	}
	if (state.thread_index == 0) {
		// Teardown code here.
		// seer::buffer.dump_to_file();
	}
}
BENCHMARK(scope_timer)->ThreadRange(1, std::thread::hardware_concurrency());

std::mutex mutex;
class SpinLock {
	std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
	void lock() {
		while (locked.test_and_set(std::memory_order_acquire)) { ; }
	}
	void unlock() {
		locked.clear(std::memory_order_release);
	}
};
SpinLock spin;
static void spin_lock(benchmark::State& state) {
	if (state.thread_index == 0) {
		seer::buffer.clear();
	}
	for (auto _ : state) {
		//seer::ScopeTimer("test");
		std::lock_guard<SpinLock> guard(spin);
	}
	if (state.thread_index == 0) {
		// Teardown code here.
		// seer::buffer.dump_to_file();
	}
}
BENCHMARK(spin_lock)->ThreadRange(1, std::thread::hardware_concurrency());


#include <string>
#include <array>
#include <cstring>
#include <iostream>
// #include <string_view>

struct Event {
	int a;
	std::string_view name;
};
static std::array<char, 200000000> buffer;
static std::size_t position = 0;

static std::size_t event_size(Event &event) {
	return sizeof(Event) + event.name.size();
}

static void insert_to_buffer(Event event) {
	std::lock_guard<SpinLock> guard(spin);

	if ((position + event_size(event)) > buffer.size()) {
		position = 0;
	}
	std::memcpy(&buffer[position + sizeof(Event)], event.name.data(), event.name.size());
	event.name = std::string_view(&buffer[position + sizeof(Event)], event.name.size());
	std::memcpy(&buffer[position], &event, sizeof(Event));
	position += sizeof(Event) + event.name.size();
	// std::copy(event.name.begin(), event.name.end(), buffer[position + sizeof(Event)]);
	// event.name = ; 
}

// int main() {
//     insert_to_buffer({1, "hello"});
//     insert_to_buffer({5, "thomas"});
//     // return sizeof(std::string_view);
//     // return buffer
//     std::cout << buffer.size();
//     for(const auto i : buffer) {
//         std::cout << i;
//     }
// }


static void bench(benchmark::State& state) {
	// Code before the loop is not measured
	for (auto _ : state) {
		insert_to_buffer({ 1, "hello" });
	}
}

BENCHMARK(bench)->ThreadRange(1, std::thread::hardware_concurrency());;

BENCHMARK_MAIN();
