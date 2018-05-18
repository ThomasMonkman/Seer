
#include "benchmark/benchmark.h"
#include "../Seer.hpp"

#include <thread>

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

BENCHMARK_MAIN();
