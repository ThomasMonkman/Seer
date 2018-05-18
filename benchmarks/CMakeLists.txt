set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

#include google benchmark
add_subdirectory(benchmark)
include_directories(benchmark/include)

#add tests
set(SEER_BENCHMARK_TARGET_NAME "seer_benchmark")
add_executable(${SEER_BENCHMARK_TARGET_NAME}
	${PROJECT_SOURCE_DIR}/benchmarks/BenchMain.cpp)

#enable pthreads for linux
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
	
target_link_libraries(${SEER_BENCHMARK_TARGET_NAME}
	benchmark
	Threads::Threads)

install(TARGETS ${SEER_BENCHMARK_TARGET_NAME} DESTINATION ${PROJECT_BINARY_DIR}/bin)