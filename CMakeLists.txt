cmake_minimum_required(VERSION 3.25.0)

project(crayfish)

add_subdirectory(engine)
add_subdirectory(testapp)

target_include_directories(testapp PUBLIC "${engine_SOURCE_DIR}/src")
target_link_directories(testapp PUBLIC "${CMAKE_SOURCE_DIR}/lib")