#add tests
set(SEER_UNIT_TEST_TARGET_NAME "seer_benchmark")
add_executable(${SEER_UNIT_TEST_TARGET_NAME}
	${PROJECT_SOURCE_DIR}/benchmarks/BenchMain.cpp)

#enable pthreads for linux
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
	
target_link_libraries(${SEER_UNIT_TEST_TARGET_NAME}
	Catch
	Threads::Threads)

install(TARGETS ${SEER_UNIT_TEST_TARGET_NAME} DESTINATION ${PROJECT_BINARY_DIR}/bin)