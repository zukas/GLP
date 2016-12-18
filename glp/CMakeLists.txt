project(glp)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/engine)
link_directories(${CMAKE_SOURCE_DIR}/engine)

file(GLOB HEADER_FILES
    "*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    )

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} engine)
