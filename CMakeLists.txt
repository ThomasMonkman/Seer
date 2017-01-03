cmake_minimum_required(VERSION 3.6)

#Project name
project( Seer VERSION 0.0.1 LANGUAGES CXX)


set(3RDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdParty")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( ${INCLUDE_DIR} )
#3rdPartys
#include_directories( "${3RDPARTY_DIR}/json" )
#include_directories( "${3RDPARTY_DIR}" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(Seer SHARED ${SOURCE_DIR}/BaseProducer.cpp ${3RDPARTY_DIR})