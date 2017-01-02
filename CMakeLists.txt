project(GLP)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)

include_directories(${CMAKE_SOURCE_DIR}/engine/vulkan_sdk/include)
link_directories(${CMAKE_SOURCE_DIR}/engine/vulkan_sdk/lib)

add_subdirectory(engine)
add_subdirectory(glp)
add_subdirectory(bench)

