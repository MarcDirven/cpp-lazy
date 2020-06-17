cmake_minimum_required(VERSION 3.16)

project("cpp-lazy")

set(CMAKE_CXX_STANDARD 14)


set(INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(HEADERS ${INCLUDE}/Lz)
set(DETAIL ${INCLUDE}/Lz/detail)

set(DETAIL_HEADER_FILES
        ${DETAIL}/EnumerateIterator.hpp
        ${DETAIL}/FilterIterator.hpp
        ${DETAIL}/LzTools.hpp
        ${DETAIL}/MapIterator.hpp
        ${DETAIL}/RangeIterator.hpp
        ${DETAIL}/SplitIterator.hpp
        ${DETAIL}/TakeIterator.hpp
        ${DETAIL}/ZipIterator.hpp
        )
set(HEADER_FILES
        ${INCLUDE}/Lz.hpp
        ${HEADERS}/Enumerate.hpp
        ${HEADERS}/Filter.hpp
        ${HEADERS}/Map.hpp
        ${HEADERS}/Range.hpp
        ${HEADERS}/StringSplitter.hpp
        ${HEADERS}/Take.hpp
        ${HEADERS}/Zip.hpp
        )

add_library("cpp-lazy" INTERFACE)

target_sources("cpp-lazy" INTERFACE
        ${HEADER_FILES}
        ${DETAIL_HEADER_FILES}
        )
target_include_directories("cpp-lazy" INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)
endif()