cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_CATCH_VERSION "2.13.9" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(Catch2 
	URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CPP-LAZY_CATCH_VERSION}.tar.gz 
	URL_MD5 feda9b6fd01621d404537d38df56ff83
)
FetchContent_MakeAvailable(Catch2)


# ---- Import root project ----
option(TEST_INSTALLED_VERSION "Import the library using find_package" OFF)
if (TEST_INSTALLED_VERSION)
	find_package(cpp-lazy REQUIRED CONFIG)
else ()
	# Enable warnings from includes
	set(cpp-lazy_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

	include(FetchContent)
	FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
	FetchContent_MakeAvailable(cpp-lazy)
endif ()

# ---- Tests ----
add_executable(LazyTests
		cartesian-product-tests.cpp
		chunk-if-tests.cpp
		chunks-tests.cpp
		concatenate-tests.cpp
		enumerate-tests.cpp
		except-tests.cpp
		exclude-tests.cpp
		filter-tests.cpp
		flatten-tests.cpp
		function-tools-tests.cpp
		generate-tests.cpp
		group-by-tests.cpp
		join-tests.cpp
		join-where-tests.cpp
		loop-tests.cpp
		lz-chain-tests.cpp
		map-tests.cpp
		random-tests.cpp
		range-tests.cpp
		repeat-tests.cpp
		rotate-tests.cpp
		standalone.cpp
		string-splitter-tests.cpp
		take-every-tests.cpp
		take-tests.cpp
		test-main.cpp
		unique-tests.cpp
		zip-tests.cpp)

target_compile_features(LazyTests PRIVATE cxx_std_11)

target_compile_options(LazyTests
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wno-unused-function>)

target_compile_definitions(LazyTests PRIVATE -DCATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag(-Wa,-mbig-obj "HAS_MBIG_OBJ")
	if (${HAS_MBIG_OBJ})
		add_definitions("-Wa,-mbig-obj")
	endif ()
endif ()

target_link_libraries(LazyTests PRIVATE cpp-lazy::cpp-lazy Catch2::Catch2)

enable_testing()

add_test(NAME LazyTests COMMAND LazyTests)
