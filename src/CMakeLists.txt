# Do not use file globing because then CMake is not able to tell whether a file
# has been deleted or added when rebuilding the project.
set(SEER_LIB_SRC
  Pipe.cpp
  BaseProducer.cpp
  WebSocket.cpp
  ScopeTimer.cpp
)
add_library(seer ${SEER_LIB_SRC})

install(TARGETS seer DESTINATION ${PROJECT_BINARY_DIR})

set(MAIN_SRC
  main.cpp
)
add_executable(seer_bin ${MAIN_SRC})
target_link_libraries(seer_bin seer)

install(TARGETS seer_bin DESTINATION ${PROJECT_BINARY_DIR})