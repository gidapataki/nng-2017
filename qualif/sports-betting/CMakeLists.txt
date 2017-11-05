cmake_minimum_required(VERSION 2.8)

project(nng-2017-sports-betting)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sports-betting-brute src/main.cpp src/Number.cpp src/solve.cpp)
add_executable(sports-betting-hungarian src/main.cpp src/Number.cpp src/hungarian_solve.cpp)
