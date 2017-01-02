project(engine)
cmake_minimum_required(VERSION 2.8)

set(VULKAN_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vulkan_sdk")

message("Vulkan SDK path: ${VULKAN_SDK_PATH}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-asynchronous-unwind-tables -fno-exceptions -fPIC")

option(GLP_SANITIZE_ADDRESS "Enable -fsanitize=address" OFF)
option(GLP_SANITIZE_KERNEL_ADDRESS "Enable -fsanitize=kernel-address" OFF)
option(GLP_SANITIZE_THREAD "Enable -fsanitize=thread" OFF)
option(GLP_SANITIZE_LEAK "Enable -fsanitize=leak" OFF)
option(GLP_SANITIZE_UNDEFINED "Enable -fsanitize=undefined" OFF)
option(GLP_PERF "Enable perf mode" OFF)
option(GLP_MEM_STAT "Enable engine memory statistics" OFF)

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

if(GLP_MEM_STAT)
    add_definitions(-DALLOCATOR_STATS)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${VULKAN_SDK_PATH}/include)
include_directories(glfw/include)
include_directories(gli)
include_directories(glm)

add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(gli)
add_subdirectory(audio)
add_subdirectory(interface)
add_subdirectory(io)
add_subdirectory(memory)
add_subdirectory(phisics)
add_subdirectory(renderer)
add_subdirectory(resource)
add_subdirectory(utils)
add_subdirectory(runtime)

file(GLOB HEADER_FILES
    "*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    )

link_directories(${VULKAN_SDK_PATH}/lib)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} audio interface io memory phisics renderer resource utils runtime)


