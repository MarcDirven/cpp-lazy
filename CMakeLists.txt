cmake_minimum_required(VERSION 3.16)
project(cpp_lazy)

include_directories(include)
include_directories(include/external)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_lazy main.cpp)