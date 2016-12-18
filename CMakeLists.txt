project(GLP)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)


add_subdirectory(engine)
add_subdirectory(glp)
add_subdirectory(bench)

