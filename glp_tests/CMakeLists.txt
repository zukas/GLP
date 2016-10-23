project(glp_tests)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/deps/glm)
include_directories(${CMAKE_SOURCE_DIR}/deps/benchmark/include)

link_directories(${CMAKE_BINARY_DIR}/deps/benchmark/src)

file(GLOB HEADER_FILES
    "*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    )

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} benchmark memory)


