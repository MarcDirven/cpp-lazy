cmake_minimum_required(VERSION 3.12.4)

project("cpp-lazy")

set(CMAKE_CXX_STANDARD 14)


set(INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(IT_HEADERS ${INCLUDE}/Lz)
set(IT_DETAIL_HEADERS ${INCLUDE}/Lz/detail)

set(DETAIL_HEADER_FILES
        ${IT_DETAIL_HEADERS}/EnumerateIterator.hpp
        ${IT_DETAIL_HEADERS}/ExceptIterator.hpp
        ${IT_DETAIL_HEADERS}/FilterIterator.hpp
        ${IT_DETAIL_HEADERS}/LzTools.hpp
        ${IT_DETAIL_HEADERS}/MapIterator.hpp
        ${IT_DETAIL_HEADERS}/RangeIterator.hpp
        ${IT_DETAIL_HEADERS}/RepeatIterator.hpp
        ${IT_DETAIL_HEADERS}/SplitIterator.hpp
        ${IT_DETAIL_HEADERS}/TakeIterator.hpp
        ${IT_DETAIL_HEADERS}/ZipIterator.hpp
        )
set(HEADER_FILES
        ${INCLUDE}/Lz.hpp
        
        ${IT_HEADERS}/Enumerate.hpp
        ${IT_HEADERS}/Except.hpp
        ${IT_HEADERS}/Filter.hpp
        ${IT_HEADERS}/Map.hpp
        ${IT_HEADERS}/Range.hpp
        ${IT_HEADERS}/Repeat.hpp
        ${IT_HEADERS}/StringSplitter.hpp
        ${IT_HEADERS}/Take.hpp
        ${IT_HEADERS}/Zip.hpp
        )

add_library("cpp-lazy" INTERFACE)

target_sources("cpp-lazy" INTERFACE
        ${HEADER_FILES}
        ${DETAIL_HEADER_FILES}
        )

target_include_directories("cpp-lazy" INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set(INCLUDE_BENCHMARKS FALSE)


if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)

    if (${INCLUDE_BENCHMARKS})
        add_subdirectory(extern/benchmark)
        add_subdirectory(bench)
    endif()

endif()