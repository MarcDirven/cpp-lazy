cmake_minimum_required(VERSION 3.12.4)

project(LazyTests)

set(CMAKE_CXX_STANDARD 14)

# Add catch
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern)

add_executable(LazyTests
        ${CMAKE_CURRENT_SOURCE_DIR}/test-main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/enumerate-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/filter-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/map-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/range-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/string-splitter-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/take-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zip-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/except-tests.cpp)

target_link_libraries(LazyTests cpp-lazy)
