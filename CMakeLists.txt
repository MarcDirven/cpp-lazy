cmake_minimum_required(VERSION 3.14)

project(cpp-lazy
        VERSION 5.1.1
        DESCRIPTION
        "A fast C++11/14/17/20 header only library for lazy evaluation and function tools"
        HOMEPAGE_URL "https://github.com/MarcDirven/cpp-lazy"
        LANGUAGES CXX)

# ---- Warning guard ----
# Protect dependents from this project's warnings if the guard isn't disabled
set(cpp-lazy_warning_guard "SYSTEM")
if(cpp-lazy_INCLUDE_WITHOUT_SYSTEM)
    set(cpp-lazy_warning_guard "")
endif()

# ---- Import {fmt} ----
option(CPP-LAZY_USE_STANDALONE "Standalone library without {fmt}" NO)
option(CPP-LAZY_USE_INSTALLED_FMT "Import {fmt} using find_package" NO)

if (CPP-LAZY_USE_STANDALONE)
	message(STATUS "Using cpp-lazy standalone version")
else()
	message(STATUS "Using cpp-lazy with {fmt} library")
	if (CPP-LAZY_USE_INSTALLED_FMT)
		find_package(fmt 8 REQUIRED CONFIG)
	else()
		set(FMT_INSTALL YES CACHE INTERNAL "" FORCE)
		set(FMT_OS YES CACHE INTERNAL "")
		include(FetchContent)
		FetchContent_Declare(fmt 
			URL https://github.com/fmtlib/fmt/archive/refs/tags/9.1.0.tar.gz 
			UPDATE_DISCONNECTED YES 
			URL_MD5 21fac48cae8f3b4a5783ae06b443973a
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
		FetchContent_MakeAvailable(fmt)
	endif()
endif()

# ---- Declare library ----
add_library(cpp-lazy INTERFACE)
add_library(cpp-lazy::cpp-lazy ALIAS cpp-lazy)

if (NOT CPP-LAZY_USE_STANDALONE)
	target_link_libraries(cpp-lazy INTERFACE fmt::fmt)
else ()
	target_compile_definitions(cpp-lazy INTERFACE LZ_STANDALONE)
endif ()

target_compile_features(cpp-lazy INTERFACE cxx_std_11)

target_include_directories(cpp-lazy
        ${cpp-lazy_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

# ---- Install ----
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(cpp-lazy_directory "cpp-lazy-${PROJECT_VERSION}")
set(cpp-lazy_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/${cpp-lazy_directory}")

install(TARGETS cpp-lazy
        EXPORT cpp-lazyTargets
        INCLUDES
        DESTINATION "${cpp-lazy_include_directory}")

write_basic_package_version_file(
        cpp-lazyConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)

set(cpp-lazy_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${cpp-lazy_directory}")

install(EXPORT cpp-lazyTargets
        NAMESPACE cpp-lazy::
        DESTINATION "${cpp-lazy_install_cmakedir}")

install(FILES
        "${PROJECT_SOURCE_DIR}/cmake/cpp-lazyConfig.cmake"
        "${PROJECT_BINARY_DIR}/cpp-lazyConfigVersion.cmake"
        DESTINATION "${cpp-lazy_install_cmakedir}")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${cpp-lazy_include_directory}")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")

include(CPack)

# ---- Release configuration zip package, bare essentials ----
option(CPP-LAZY_BUILD_ESSENTIALS_ZIP OFF)
if (CPP-LAZY_BUILD_ESSENTIALS_ZIP)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip
		COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip --format=zip -- include/ CMakeLists.txt LICENSE.md cmake/
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS include/;CMakeLists.txt;LICENSE.md;cmake/
	)
	add_custom_target(${PROJECT_NAME}_package DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip)
endif ()