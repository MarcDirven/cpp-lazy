cmake_minimum_required(VERSION 3.12.4)

project(cpp-lazy
        VERSION 2.0.1
        DESCRIPTION
        "A fast C++11/14/17/20 header only library for lazy evaluation and function tools"
        HOMEPAGE_URL "https://github.com/MarcDirven/cpp-lazy"
        LANGUAGES CXX)


set(INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LZ_HEADERS ${INCLUDE}/Lz)
set(LZ_DETAIL_HEADERS ${INCLUDE}/Lz/detail)

set(DETAIL_HEADER_FILES
        ${LZ_DETAIL_HEADERS}/BasicIteratorView.hpp
        ${LZ_DETAIL_HEADERS}/ChooseIterator.hpp
        ${LZ_DETAIL_HEADERS}/ConcatenateIterator.hpp
        ${LZ_DETAIL_HEADERS}/EnumerateIterator.hpp
        ${LZ_DETAIL_HEADERS}/ExceptIterator.hpp
        ${LZ_DETAIL_HEADERS}/FilterIterator.hpp
        ${LZ_DETAIL_HEADERS}/GenerateIterator.hpp
        ${LZ_DETAIL_HEADERS}/LzTools.hpp
        ${LZ_DETAIL_HEADERS}/MapIterator.hpp
        ${LZ_DETAIL_HEADERS}/RandomIterator.hpp
        ${LZ_DETAIL_HEADERS}/RangeIterator.hpp
        ${LZ_DETAIL_HEADERS}/RepeatIterator.hpp
        ${LZ_DETAIL_HEADERS}/SplitIterator.hpp
        ${LZ_DETAIL_HEADERS}/TakeIterator.hpp
        ${LZ_DETAIL_HEADERS}/TakeEveryIterator.hpp
        ${LZ_DETAIL_HEADERS}/ZipIterator.hpp
        )

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

        ${LZ_HEADERS}/Choose.hpp
        ${LZ_HEADERS}/Concatenate.hpp
        ${LZ_HEADERS}/Enumerate.hpp
        ${LZ_HEADERS}/Except.hpp
        ${LZ_HEADERS}/Filter.hpp
        ${LZ_HEADERS}/Generate.hpp
        ${LZ_HEADERS}/Map.hpp
        ${LZ_HEADERS}/Random.hpp
        ${LZ_HEADERS}/Range.hpp
        ${LZ_HEADERS}/Repeat.hpp
        ${LZ_HEADERS}/StringSplitter.hpp
        ${LZ_HEADERS}/Take.hpp
        ${LZ_HEADERS}/TakeEvery.hpp
        ${LZ_HEADERS}/Zip.hpp
        )

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_sources("cpp-lazy" INTERFACE
        ${HEADER_FILES}
        ${DETAIL_HEADER_FILES}
        )

target_include_directories("cpp-lazy" INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set(INCLUDE_BENCHMARKS TRUE)


if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)
    add_subdirectory(examples)
    # Use a boolean for the benchmarks for now
    set(INCLUDE_BENCHMARKS TRUE)
    if (${INCLUDE_BENCHMARKS})
        add_subdirectory(extern/benchmark)
        add_subdirectory(bench)
    endif()

endif()