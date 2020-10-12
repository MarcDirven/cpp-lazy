cmake_minimum_required(VERSION 3.12.4)

project(Benchmark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Benchmark
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks-iterators.cpp)

# Add cpp-lazy
option(TEST_INSTALLED_VERSION "Import the library using find_package" OFF)
if(TEST_INSTALLED_VERSION)
    find_package(cpp-lazy REQUIRED CONFIG)
else()
    # Enable warnings from includes
    set(cpp-lazy_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

    include(FetchContent)
    FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
    FetchContent_MakeAvailable(cpp-lazy)
endif()

include(FetchContent)
# Enable Google Benchmark to add dependencies for their tests
set(BENCHMARK_DOWNLOAD_DEPENDENCIES TRUE)
FetchContent_Declare(benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG ffe1342eb2faa7d2e7c35b4db2ccf99fab81ec20
        UPDATE_DISCONNECTED YES)
FetchContent_MakeAvailable(benchmark)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")
target_link_libraries(Benchmark
        cpp-lazy
        benchmark::benchmark
        )
