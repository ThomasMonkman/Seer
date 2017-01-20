cmake_minimum_required(VERSION 3.6)

#Project name
project( Seer VERSION 0.0.1 LANGUAGES CXX)
include_directories("${CMAKE_SOURCE_DIR}/json"
"${CMAKE_SOURCE_DIR}/catch")

set(HEADER_FILES 
3rdParty/json/json.hpp
3rdParty/catch/catch.hpp
include/BaseProducer.hpp
include/DataPoint.hpp
include/Network.hpp
include/ScopeTimer.hpp)

set(SOURCE_FILES 
src/BaseProducer.cpp
src/Network.cpp
src/ScopeTimer.cpp)

#add_subdirectory(tests)

add_library(Seer SHARED ${SOURCE_FILES} ${HEADER_FILES})


#add_library(Seer SHARED 

#set(3RDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdParty")
#set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
#set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

#include_directories( ${INCLUDE_DIR} )
#3rdPartys
#include_directories( "${3RDPARTY_DIR}" )

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#add_library(Seer SHARED 
#"${SOURCE_DIR}/BaseProducer.cpp"
#"${SOURCE_DIR}/Network.cpp"
#"${SOURCE_DIR}/ScopeTimer.cpp"
#"${3RDPARTY_DIR}")