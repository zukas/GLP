project(engine)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti  -fno-asynchronous-unwind-tables -fno-exceptions")

option(GLP_SANITIZE_ADDRESS "Enable -fsanitize=address" OFF)
option(GLP_SANITIZE_KERNEL_ADDRESS "Enable -fsanitize=kernel-address" OFF)
option(GLP_SANITIZE_THREAD "Enable -fsanitize=thread" OFF)
option(GLP_SANITIZE_LEAK "Enable -fsanitize=leak" OFF)
option(GLP_SANITIZE_UNDEFINED "Enable -fsanitize=undefined" OFF)
option(GLP_PERF "Enable perf mode" OFF)

if(GLP_SANITIZE_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if(GLP_SANITIZE_KERNEL_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=kernel-address")
endif()

if(GLP_SANITIZE_THREAD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

if(GLP_SANITIZE_LEAK)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif()

if(GLP_SANITIZE_UNDEFINED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if(GLP_PERF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -fno-omit-frame-pointer")
endif()


add_subdirectory(glm)
add_subdirectory(glfw)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
message("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(glfw/include)
include_directories(glm)

file(GLOB HEADER_FILES
    "*.h"
    "memory/*.h"
    "render/*.h"
    "render/vulkan/*.h"
    "phisics/*.h"
    "audio/*.h"
    "io/*.h"
    "interface/*.h"
    "resource/*.h"
    "utils/*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    "memory/*.cpp"
    "render/*.cpp"
    "render/vulkan/*.cpp"
    "phisics/*.cpp"
    "audio/*.cpp"
    "io/*.cpp"
    "interface/*.cpp"
    "resource/*.cpp"
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} glfw)
