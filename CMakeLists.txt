MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
# this is the directory where the currently processed CMakeLists.txt is located in 
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )
####################################################################################

cmake_minimum_required(VERSION 3.1)

# Project name
project( Seer VERSION 0.0.1 LANGUAGES C CXX)

option(BuildTests "Build the unit tests" ON)
enable_testing()

# Enable c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Add debug flags
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /MP")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/3rdParty/websocketpp
  ${PROJECT_SOURCE_DIR}/3rdParty/asio/include
  ${PROJECT_SOURCE_DIR}/3rdParty
  ${PROJECT_SOURCE_DIR}/3rdParty/catch
  ${PROJECT_SOURCE_DIR}/3rdParty/json
)

add_subdirectory(src)

# create and configure the unit test target
if(BuildTests)
    add_subdirectory(tests)
endif()

####################################################################################
