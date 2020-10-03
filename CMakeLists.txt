cmake_minimum_required(VERSION 3.13)

project(cpp-lazy
        VERSION 2.0.0
        DESCRIPTION
        "A fast C++14/17/20 header only library for lazy evaluation and function tools"
        HOMEPAGE_URL "https://github.com/MarcDirven/cpp-lazy"
        LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)

add_subdirectory(include/Lz/fmt)
target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(CPP_LAZY_INSTALL_LIBRARY
        "Enable installing of cpp-lazy library"
        ${IS_TOPLEVEL_PROJECT})

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Add fmt
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

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)

if (CPP_LAZY_INSTALL_LIBRARY)
    install(TARGETS ${PROJECT_NAME} fmt
            EXPORT ${PROJECT_NAME}_Targets
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
            "${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion)

    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" INSTALL_DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

    install(
            EXPORT ${PROJECT_NAME}_Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/Lz"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")

include(CPack)
