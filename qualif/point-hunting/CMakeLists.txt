cmake_minimum_required(VERSION 2.8)

project(nng-2017-point-hunting)

add_definitions(-DDEBUG)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -O3")

add_executable(testing main.cpp Cube.cpp Field.cpp)
