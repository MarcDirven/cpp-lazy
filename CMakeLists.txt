cmake_minimum_required(VERSION 3.14)

project(cpp-lazy
        VERSION 7.0.2
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

include(GNUInstallDirs)
set(CPP-LAZY_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE STRING "Installation directory of the include files")

option(CPP-LAZY_USE_MODULES "Enable C++20's modules" NO)

if (CPP-LAZY_USE_MODULES)
	set(CPP-LAZY_SOURCE_FILES "src/lz.cppm")
	set(CPP-LAZY_LINK_VISIBILITY "PUBLIC")
	set(CPP-LAZY_COMPILE_FEATURES_VISIBLITY "PUBLIC")
	set(CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY "PRIVATE")
	set(CPP-LAZY_COMPILE_INCLUDE_VISIBLITY "PUBLIC")
else()
	set(CPP-LAZY_LIB_TYPE "INTERFACE")
	set(CPP-LAZY_LINK_VISIBILITY "INTERFACE")
	set(CPP-LAZY_COMPILE_FEATURES_VISIBLITY "INTERFACE")
	set(CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY "INTERFACE")
	set(CPP-LAZY_COMPILE_INCLUDE_VISIBLITY "INTERFACE")
endif()

# ---- Import {fmt} ----
option(CPP-LAZY_USE_STANDALONE "Standalone library without {fmt}" NO)
option(CPP-LAZY_USE_INSTALLED_FMT "Import {fmt} using find_package" NO)

if (CPP-LAZY_USE_STANDALONE)
	message(STATUS "Using cpp-lazy standalone version")
else()
	message(STATUS "Using cpp-lazy with {fmt} library")
	if (CPP-LAZY_USE_INSTALLED_FMT)
		find_package(fmt REQUIRED CONFIG)
	else()
		set(FMT_OS YES CACHE INTERNAL "")
		set(FMT_INSTALL YES CACHE INTERNAL "" FORCE)
		include(FetchContent)
		FetchContent_Declare(fmt 
			URL https://github.com/fmtlib/fmt/archive/refs/tags/10.2.1.tar.gz 
			UPDATE_DISCONNECTED YES 
			URL_MD5 dc09168c94f90ea890257995f2c497a5
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
		FetchContent_MakeAvailable(fmt)
	endif()
endif()

# ---- Declare library ----
add_library(cpp-lazy ${CPP-LAZY_LIB_TYPE} "${CPP-LAZY_SOURCE_FILES}")
add_library(cpp-lazy::cpp-lazy ALIAS cpp-lazy)

target_link_libraries(cpp-lazy 
	${CPP-LAZY_LINK_VISIBILITY}
		$<$<NOT:$<BOOL:${CPP-LAZY_USE_STANDALONE}>>:fmt::fmt>
)
target_compile_definitions(cpp-lazy 
	${CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY}
		$<$<BOOL:${CPP-LAZY_USE_STANDALONE}>:LZ_STANDALONE>
)
if (CPP-LAZY_USE_MODULES)
	set(FMT_MODULE "ON" CACHE STRING "")
	if (MSVC)
		set(BMI "${CMAKE_CURRENT_BINARY_DIR}/lz.ifc")
		target_compile_options(cpp-lazy
			PRIVATE
				/interface /ifcOutput "${BMI}"
			INTERFACE
				/reference lz="${BMI}"
		)
	endif()
	set_target_properties(cpp-lazy 
		PROPERTIES 
			ADDITIONAL_CLEAN_FILES "${BMI}"
		DEBUG_POSTFIX
			"d"
	)
	set_source_files_properties("${BMI}" PROPERTIES GENERATED ON)
endif()

target_compile_features(cpp-lazy 
	${CPP-LAZY_COMPILE_FEATURES_VISIBLITY} 
		cxx_std_11
)
target_include_directories(cpp-lazy
	${cpp-lazy_warning_guard}
		${CPP-LAZY_COMPILE_INCLUDE_VISIBLITY}
        	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
			"$<INSTALL_INTERFACE:${CPP-LAZY_INCLUDE_DIR}>"
)

# ---- Install ----
include(CMakePackageConfigHelpers)

set(cpp-lazy_directory "cpp-lazy-${PROJECT_VERSION}")
set(cpp-lazy_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/${cpp-lazy_directory}")
set(cpp-lazy_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${cpp-lazy_directory}")

install(TARGETS cpp-lazy
        EXPORT cpp-lazyTargets
        INCLUDES
        DESTINATION "${cpp-lazy_include_directory}")

write_basic_package_version_file(
        cpp-lazyConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)


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
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip"
		COMMAND ${CMAKE_COMMAND} -E tar c "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip" --format=zip -- include/ CMakeLists.txt LICENSE.md cmake/
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		DEPENDS include/;CMakeLists.txt;LICENSE.md;cmake/
	)
	add_custom_target(${PROJECT_NAME}_package DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip")
endif ()