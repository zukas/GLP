project(renderer)
cmake_minimum_required(VERSION 2.8)

add_definitions(-DGLFW_INCLUDE_VULKAN)

file(GLOB HEADER_FILES
    "*.h"
    "primitives/*.h"
    "vk/*.h"
    "vk/internal/*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    "primitives/*.cpp"
    "vk/*.cpp"
    )


add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} vulkan glfw memory)
