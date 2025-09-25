cmake_minimum_required(VERSION 3.10)
project(simple_threaded_server)

set(CMAKE_CXX_STANDARD 20)
set(SOURCES src/thread_pool.cpp src/server.cpp)

include_directories(include)
add_executable(server ${SOURCES})
target_link_libraries(server pthread)
