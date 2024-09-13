cmake_minimum_required(VERSION 3.25)
project(threadpool)

set(CMAKE_CXX_STANDARD 17)

message("info" ${PROJECT_SOURCE_DIR})

add_executable(threadpool example.cc)
