cmake_minimum_required(VERSION 3.6)

#Project name
project( Seer VERSION 0.0.1 LANGUAGES CXX )

# Add debug flags
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/3rdParty/websocketpp
  ${PROJECT_SOURCE_DIR}/3rdParty/asio/include
  ${PROJECT_SOURCE_DIR}/3rdParty
)

add_subdirectory(src)