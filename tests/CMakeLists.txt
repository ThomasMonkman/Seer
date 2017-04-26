#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#add_library(catch interface)
#target_include_directories(catch interface 3rdparty/catch/catch.hpp)

#include catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/catch/catch.hpp)

#add tests
set(SEER_UNIT_TEST_TARGET_NAME "seer_unit")
    #$<TARGET_OBJECTS:catch_main>
add_executable(${SEER_UNIT_TEST_TARGET_NAME}
	${PROJECT_SOURCE_DIR}/tests/src/CatchMain.cpp
	${PROJECT_SOURCE_DIR}/tests/src/TestMain.cpp)

#target_include_directories(${SEER_UNIT_TEST_TARGET_NAME} PRIVATE "src" "${PROJECT_SOURCE_DIR}/3rdParty/catch/catch.hpp")


target_link_libraries(${SEER_UNIT_TEST_TARGET_NAME}
	Catch)

add_test(NAME "${SEER_UNIT_TEST_TARGET_NAME}_default"
	COMMAND ${SEER_UNIT_TEST_TARGET_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(TARGETS ${SEER_UNIT_TEST_TARGET_NAME} DESTINATION ${PROJECT_BINARY_DIR})