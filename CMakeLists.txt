cmake_minimum_required(VERSION 3.12.4)

project(cpp-lazy VERSION 2.0.0)
set(CMAKE_CXX_STANDARD 20)


add_library(cpp-lazy INTERFACE)
add_library(cpp-lazy::cpp-lazy ALIAS cpp-lazy)

include(GNUInstallDirs)

target_include_directories(cpp-lazy INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Add fmt
add_subdirectory(extern/fmt)
target_link_libraries(cpp-lazy INTERFACE fmt::fmt-header-only)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)
    add_subdirectory(examples)

    # Use a boolean for the benchmarks for now
    set(INCLUDE_BENCHMARKS FALSE)
    if (${INCLUDE_BENCHMARKS})
        add_subdirectory(extern/benchmark)
        add_subdirectory(bench)
    endif()

endif()

install(TARGETS cpp-lazy fmt-header-only
        EXPORT cpp-lazy_Targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(cpp-lazyConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/cpp-lazyConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/cpp-lazyConfig.cmake"
        INSTALL_DESTINATION
        "${CMAKE_INSTALL_DATAROOTDIR}/cpp-lazy/cmake")

install(EXPORT cpp-lazy_Targets
        FILE cpp-lazyTargets.cmake
        NAMESPACE cpp-lazy::
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cpp-lazy/cmake")

install(FILES
        "${PROJECT_BINARY_DIR}/cpp-lazyConfig.cmake"
        "${PROJECT_BINARY_DIR}/cpp-lazyConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cpp-lazy/cmake")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/cpp-lazy" DESTINATION include)
