cmake_minimum_required(VERSION 3.10...3.12.4)

project(Benchmark)
set(CMAKE_CXX_VERSION 17)
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
        URL https://github.com/google/benchmark/archive/refs/tags/v1.8.4.tar.gz
        UPDATE_DISCONNECTED YES
        URL_MD5 6b1423a35c5e2b2bb9ff6bbf2a897000
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
FetchContent_MakeAvailable(benchmark)

target_link_libraries(Benchmark
        cpp-lazy
        benchmark::benchmark
)

