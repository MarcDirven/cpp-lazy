cmake_minimum_required(VERSION 3.12.4)
set(CMAKE_CXX_STANDARD 11)

project(LazyTests)

add_executable(LazyTests
        ${CMAKE_CURRENT_SOURCE_DIR}/affirm-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concatenate-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/enumerate-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/except-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/filter-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/function-tools-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generate-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/join-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/map-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/random-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/range-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/repeat-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/string-splitter-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/take-every-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/take-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test-main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/unique-tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zip-tests.cpp
        )

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Remove error cl : Command line warning D9025 : overriding '/W3' with '/W4'
    string(REPLACE "/W3" "/W4" FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS})

# Add catch
target_include_directories(LazyTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern)
target_link_libraries(LazyTests cpp-lazy::cpp-lazy)
