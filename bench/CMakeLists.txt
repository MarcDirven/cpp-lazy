cmake_minimum_required(VERSION 3.16)

project(Benchmark)

set(CMAKE_CXX_STANDARD 14)

add_executable(Benchmark ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks.cpp)

target_link_libraries(Benchmark
        cpp-lazy
        benchmark::benchmark
        )

