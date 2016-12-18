project(backend-vk)
cmake_minimum_required(VERSION 2.8)

file(GLOB HEADER_FILES
    "*.h"

    )

file(GLOB SOURCE_FILES
    "*.cpp"
    )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/modules")

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} glfw ${VULKAN_LIBRARY} renderer)
