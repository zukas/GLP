project(GLP)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti  -fno-asynchronous-unwind-tables")

option(GLP_SANITIZE_UNDEFINED "Enable -fsanitize=undefined" OFF)

option(GLP_PERF "Enable perf mode" OFF)

if(GLP_PERF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -fno-omit-frame-pointer")
endif()

add_subdirectory(deps/benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

add_subdirectory(deps/glm)
add_subdirectory(deps/glfw)
add_subdirectory(memory)
add_subdirectory(engine)

add_subdirectory(glp_bench)
add_subdirectory(glp_unit)

