#include catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/catch/catch.hpp)

#add tests
set(SEER_UNIT_TEST_TARGET_NAME "seer_unit")
add_executable(${SEER_UNIT_TEST_TARGET_NAME}
	${PROJECT_SOURCE_DIR}/tests/src/CatchMain.cpp
	${PROJECT_SOURCE_DIR}/tests/src/TestMain.cpp
	${PROJECT_SOURCE_DIR}/tests/src/Pipe.spec.cpp
	${PROJECT_SOURCE_DIR}/tests/src/Sink.spec.cpp
	${PROJECT_SOURCE_DIR}/tests/src/ScopeTimer.spec.cpp
	${PROJECT_SOURCE_DIR}/tests/src/Timer.spec.cpp)

#enable pthreads for linux
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
	
target_link_libraries(${SEER_UNIT_TEST_TARGET_NAME}
	seer
	Catch
	Threads::Threads)

add_test(NAME "${SEER_UNIT_TEST_TARGET_NAME}_default"
	COMMAND ${SEER_UNIT_TEST_TARGET_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(TARGETS ${SEER_UNIT_TEST_TARGET_NAME} DESTINATION ${PROJECT_BINARY_DIR}/bin)