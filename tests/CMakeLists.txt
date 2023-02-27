cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_CATCH_VERSION "2.13.9" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(Catch2 
	URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CPP-LAZY_CATCH_VERSION}.tar.gz 
	URL_MD5 feda9b6fd01621d404537d38df56ff83
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

function(cpp_lazy_add_test _target)
	add_executable(${_target} ${_target}.cpp)
	target_compile_definitions(${_target} PRIVATE CATCH_CONFIG_MAIN)
	target_compile_options(${_target}
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion>)
	target_link_libraries(${_target} PRIVATE cpp-lazy::cpp-lazy Catch2::Catch2)
	add_test(NAME ${_target} COMMAND $<TARGET_FILE:${_target}>)
endfunction()

include(CTest)

# ---- Tests ----
cpp_lazy_add_test(cartesian-product-tests)
cpp_lazy_add_test(chunk-if-tests)
cpp_lazy_add_test(chunks-tests)
cpp_lazy_add_test(concatenate-tests)
cpp_lazy_add_test(cstring-tests)
cpp_lazy_add_test(enumerate-tests)
cpp_lazy_add_test(except-tests)
cpp_lazy_add_test(exclude-tests)
cpp_lazy_add_test(filter-tests)
cpp_lazy_add_test(flatten-tests)
cpp_lazy_add_test(function-tools-tests)
cpp_lazy_add_test(generate-tests)
cpp_lazy_add_test(generate-while-tests)
cpp_lazy_add_test(group-by-tests)
cpp_lazy_add_test(join-tests)
cpp_lazy_add_test(join-where-tests)
cpp_lazy_add_test(loop-tests)
cpp_lazy_add_test(lz-chain-tests)
cpp_lazy_add_test(map-tests)
cpp_lazy_add_test(random-tests)
cpp_lazy_add_test(range-tests)
cpp_lazy_add_test(repeat-tests)
cpp_lazy_add_test(rotate-tests)
cpp_lazy_add_test(standalone-tests)
cpp_lazy_add_test(string-splitter-tests)
cpp_lazy_add_test(take-every-tests)
cpp_lazy_add_test(take-tests)
cpp_lazy_add_test(unique-tests)
cpp_lazy_add_test(zip-longest-tests)
cpp_lazy_add_test(zip-tests)
