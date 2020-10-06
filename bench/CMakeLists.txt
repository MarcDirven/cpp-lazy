cmake_minimum_required(VERSION 3.12.4)

project(Benchmark)

set(CMAKE_CXX_STANDARD 20)

add_executable(Benchmark
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks-iterators.cpp)

target_link_libraries(Benchmark
        cpp-lazy
        benchmark::benchmark
        )
