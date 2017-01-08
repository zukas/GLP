project(GLP)
cmake_minimum_required(VERSION 2.8)


option(GLP_SANITIZE_ADDRESS "Enable -fsanitize=address" OFF)
option(GLP_SANITIZE_KERNEL_ADDRESS "Enable -fsanitize=kernel-address" OFF)
option(GLP_SANITIZE_THREAD "Enable -fsanitize=thread" OFF)
option(GLP_SANITIZE_LEAK "Enable -fsanitize=leak" OFF)
option(GLP_SANITIZE_UNDEFINED "Enable -fsanitize=undefined" OFF)
set(GLOBAL_LIBS)

if(GLP_SANITIZE_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    list(APPEND GLOBAL_LIBS asan)
endif()

if(GLP_SANITIZE_KERNEL_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=kernel-address")
endif()

if(GLP_SANITIZE_THREAD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    list(APPEND GLOBAL_LIBS tsan)
endif()

if(GLP_SANITIZE_LEAK)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
    list(APPEND GLOBAL_LIBS lsan)
endif()

if(GLP_SANITIZE_UNDEFINED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    list(APPEND GLOBAL_LIBS ubsan)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)

include_directories(${CMAKE_SOURCE_DIR}/engine/vulkan_sdk/include)
link_directories(${CMAKE_SOURCE_DIR}/engine/vulkan_sdk/lib)

add_subdirectory(engine)
add_subdirectory(glp)
#add_subdirectory(bench)

