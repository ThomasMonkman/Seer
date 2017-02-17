cmake_minimum_required(VERSION 3.6)

#Project name
project( Seer VERSION 0.0.1 LANGUAGES CXX )

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(src)