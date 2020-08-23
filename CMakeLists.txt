cmake_minimum_required(VERSION 3.12.4)

project("cpp-lazy")

set(CMAKE_CXX_STANDARD 14)


set(INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LZ_HEADERS ${INCLUDE}/Lz)
set(LZ_DETAIL_HEADERS ${INCLUDE}/Lz/detail)

set(DETAIL_HEADER_FILES
        ${LZ_DETAIL_HEADERS}/AffirmIterator.hpp
        ${LZ_DETAIL_HEADERS}/BasicIteratorView.hpp
        ${LZ_DETAIL_HEADERS}/ChooseIterator.hpp
        ${LZ_DETAIL_HEADERS}/ConcatenateIterator.hpp
        ${LZ_DETAIL_HEADERS}/DropWhileIterator.hpp
        ${LZ_DETAIL_HEADERS}/EnumerateIterator.hpp
        ${LZ_DETAIL_HEADERS}/ExceptIterator.hpp
        ${LZ_DETAIL_HEADERS}/FilterIterator.hpp
        ${LZ_DETAIL_HEADERS}/GenerateIterator.hpp
        ${LZ_DETAIL_HEADERS}/JoinIterator.hpp
        ${LZ_DETAIL_HEADERS}/LzTools.hpp
        ${LZ_DETAIL_HEADERS}/MapIterator.hpp
        ${LZ_DETAIL_HEADERS}/RandomIterator.hpp
        ${LZ_DETAIL_HEADERS}/RangeIterator.hpp
        ${LZ_DETAIL_HEADERS}/RepeatIterator.hpp
        ${LZ_DETAIL_HEADERS}/SplitIterator.hpp
        ${LZ_DETAIL_HEADERS}/TakeIterator.hpp
        ${LZ_DETAIL_HEADERS}/TakeEveryIterator.hpp
        ${LZ_DETAIL_HEADERS}/UniqueIterator.hpp
        ${LZ_DETAIL_HEADERS}/ZipIterator.hpp
        )

set(HEADER_FILES
        ${INCLUDE}/Lz.hpp

        ${LZ_HEADERS}/Affirm.hpp
        ${LZ_HEADERS}/Choose.hpp
        ${LZ_HEADERS}/Concatenate.hpp
        ${LZ_HEADERS}/DropWhile.hpp
        ${LZ_HEADERS}/Enumerate.hpp
        ${LZ_HEADERS}/Except.hpp
        ${LZ_HEADERS}/Filter.hpp
        ${LZ_HEADERS}/Generate.hpp
        ${LZ_HEADERS}/Join.hpp
        ${LZ_HEADERS}/Map.hpp
        ${LZ_HEADERS}/Random.hpp
        ${LZ_HEADERS}/Range.hpp
        ${LZ_HEADERS}/Repeat.hpp
        ${LZ_HEADERS}/StringSplitter.hpp
        ${LZ_HEADERS}/Take.hpp
        ${LZ_HEADERS}/TakeEvery.hpp
        ${LZ_HEADERS}/Unique.hpp
        ${LZ_HEADERS}/Zip.hpp
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

# Add fmt
add_subdirectory(extern/fmt)
target_link_libraries("cpp-lazy" INTERFACE fmt::fmt-header-only)


if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)
    add_subdirectory(examples)

    if (${INCLUDE_BENCHMARKS})
        add_subdirectory(extern/benchmark)
        add_subdirectory(bench)
    endif()

endif()