cmake_minimum_required(VERSION 2.8)

project(nng-2017-sports-betting)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sports-betting src/main.cpp src/Number.cpp src/solve.cpp)
