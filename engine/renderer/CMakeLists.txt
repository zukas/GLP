project(renderer)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(opengl)
add_subdirectory(vulkan)

file(GLOB HEADER_FILES
    "*.h"
    "geometry/*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    "geometry/*.cpp"
    )

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} dl io)
