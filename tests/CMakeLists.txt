#include catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/catch/catch.hpp)

#add tests
set(SEER_UNIT_TEST_TARGET_NAME "seer_unit")
add_executable(${SEER_UNIT_TEST_TARGET_NAME}
	${PROJECT_SOURCE_DIR}/tests/src/CatchMain.cpp
	${PROJECT_SOURCE_DIR}/tests/src/TestMain.cpp)
	
target_link_libraries(${SEER_UNIT_TEST_TARGET_NAME}
	Catch)

add_test(NAME "${SEER_UNIT_TEST_TARGET_NAME}_default"
	COMMAND ${SEER_UNIT_TEST_TARGET_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(TARGETS ${SEER_UNIT_TEST_TARGET_NAME} DESTINATION ${PROJECT_BINARY_DIR}/bin)