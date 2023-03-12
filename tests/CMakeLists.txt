cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_CATCH_VERSION "2.13.10" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(Catch2 
	URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CPP-LAZY_CATCH_VERSION}.tar.gz 
	URL_MD5 7a4dd2fd14fb9f46198eb670ac7834b7
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Catch2)


# ---- Import root project ----
option(TEST_INSTALLED_VERSION "Import the library using find_package" OFF)
if (TEST_INSTALLED_VERSION)
	find_package(cpp-lazy REQUIRED CONFIG)
else ()
	# Enable warnings from includes
	set(cpp-lazy_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

	FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
	FetchContent_MakeAvailable(cpp-lazy)
endif ()

include(CTest)

# ---- Tests ----
add_executable(cpp-lazy-tests
	cartesian-product-tests.cpp
	chunk-if-tests.cpp
	chunks-tests.cpp
	concatenate-tests.cpp
	cstring-tests.cpp
	enumerate-tests.cpp
	except-tests.cpp
	exclude-tests.cpp
	exclusive-scan-tests.cpp
	filter-tests.cpp
	flatten-tests.cpp
	function-tools-tests.cpp
	generate-tests.cpp
	generate-while-tests.cpp
	group-by-tests.cpp
	inclusive-scan-tests.cpp
	init-tests.cpp
	join-tests.cpp
	join-where-tests.cpp
	loop-tests.cpp
	lz-chain-tests.cpp
	map-tests.cpp
	random-tests.cpp
	range-tests.cpp
	repeat-tests.cpp
	rotate-tests.cpp
	standalone-tests.cpp
	string-splitter-tests.cpp
	take-every-tests.cpp
	take-tests.cpp
	unique-tests.cpp
	zip-longest-tests.cpp
	zip-tests.cpp
)
target_compile_options(cpp-lazy-tests
	PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX /diagnostics:caret>
	$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion>
)
target_link_libraries(cpp-lazy-tests
	PRIVATE
		cpp-lazy::cpp-lazy
		Catch2::Catch2
)
add_test(
	NAME cpp-lazy-tests
	COMMAND $<TARGET_FILE:cpp-lazy-tests>
)