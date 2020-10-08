cmake_minimum_required(VERSION 3.12)

project(build_all LANGUAGES CXX)

# Missing dependency?
# add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../bench" "${CMAKE_BINARY_DIR}/bench")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../tests" "${CMAKE_BINARY_DIR}/tests")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../examples" "${CMAKE_BINARY_DIR}/examples")
