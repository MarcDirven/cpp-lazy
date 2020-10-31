cmake_minimum_required(VERSION 3.14)

project(examples LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
FetchContent_MakeAvailable(cpp-lazy)

set(examples
        Affirm
        Concatenate
        Enumerate
        Except
        Filter
        Flatten
        FunctionTools
        Generate
        Join
        Map
        Random
        Range
        Repeat
        StringSplitter
        Take
        TakeEvery
        ToContainer
        Unique
        Zip)

foreach(name IN LISTS examples)
    add_executable(IteratorExample${name} ${name}.cpp)
    target_link_libraries(IteratorExample${name} PRIVATE cpp-lazy::cpp-lazy)
    target_compile_features(IteratorExample${name} PRIVATE cxx_std_14)
endforeach()
