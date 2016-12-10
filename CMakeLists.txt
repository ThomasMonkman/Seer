cmake_minimum_required(VERSION 3.6)

#Project name
project( Seer )


set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( ${INCLUDE_DIR} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(Seer SHARED ${SOURCE_DIR}/BaseProducer.cpp)