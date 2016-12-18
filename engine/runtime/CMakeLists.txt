project(runtime)
cmake_minimum_required(VERSION 2.8)

file(GLOB HEADER_FILES
    "*.h"
    )

file(GLOB SOURCE_FILES
    "*.cpp"
    )

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} utils memory io audio resource renderer interface phisics)
