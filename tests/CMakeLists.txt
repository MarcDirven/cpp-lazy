cmake_minimum_required(VERSION 3.16)

project(LazyTests)

set(CMAKE_CXX_STANDARD 14)

# Add catch
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern)

add_executable(LazyTests
        ${CMAKE_CURRENT_SOURCE_DIR}/test-main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/enumerate-tests.cpp filter-tests.cpp)

target_link_libraries(LazyTests cpp-lazy)