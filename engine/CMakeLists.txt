project(engine)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADER_FILES
    "*.h"
    "memory/*.h"
    "render/*.h"
    "phisics/*.h"
    "audio/*.h"
    "io/*.h"
    "interface/*.h"
    "resource/*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    "memory/*.cpp"
    "render/*.cpp"
    "phisics/*.cpp"
    "audio/*.cpp"
    "io/*.cpp"
    "interface/*.cpp"
    "resource/*.cpp"
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
