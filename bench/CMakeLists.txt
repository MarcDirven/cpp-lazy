cmake_minimum_required(VERSION 3.12.4)

project(Benchmark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Benchmark
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks-iterators.cpp)


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")
target_link_libraries(Benchmark
        cpp-lazy
        benchmark::benchmark
        )
