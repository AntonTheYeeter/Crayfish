cmake_minimum_required(VERSION 3.25.0)

project(testapp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR} "src/*.c")
add_executable(testapp ${SRC})

target_link_libraries(testapp PUBLIC engine)

add_definitions(-D_DEBUG)