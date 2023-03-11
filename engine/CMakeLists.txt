cmake_minimum_required(VERSION 3.25.0)

project(engine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR} "src/*.c")
add_library(engine SHARED ${SRC})

if(WIN32)
target_link_libraries(engine PUBLIC user32)
endif(WIN32)

if(LINUX)
target_link_libraries(engine PUBLIC xcb)
endif(LINUX)

find_package(Vulkan REQUIRED)

target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(engine PUBLIC ${Vulkan_LIBRARIES})

add_definitions(-D_DEBUG -DCFEXPORT)