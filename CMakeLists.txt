cmake_minimum_required(VERSION 3.14)

project(cpp-lazy
        VERSION 2.0.0
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

option(CPP-LAZY_USE_INSTALLED_FMT "Import {fmt} using find_package" NO)
if(CPP-LAZY_USE_INSTALLED_FMT)
    find_package(fmt 7 REQUIRED CONFIG)
else()
    set(FMT_INSTALL YES CACHE INTERNAL "" FORCE)
    set(FMT_OS YES CACHE INTERNAL "")
    include(FetchContent)
    FetchContent_Declare(fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG f468b203add3b04fb8fac473ec7a04ed64c429c1
            UPDATE_DISCONNECTED YES)
    FetchContent_MakeAvailable(fmt)
endif()

# ---- Declare library ----

add_library(cpp-lazy INTERFACE)
add_library(cpp-lazy::cpp-lazy ALIAS cpp-lazy)

target_link_libraries(cpp-lazy INTERFACE fmt::fmt)

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
        INCLUDES #
        DESTINATION "${cpp-lazy_include_directory}")

write_basic_package_version_file(
        cpp-lazyConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)

set(cpp-lazy_install_cmakedir
        "${CMAKE_INSTALL_LIBDIR}/cmake/${cpp-lazy_directory}")

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
