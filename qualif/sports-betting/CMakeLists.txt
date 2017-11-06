cmake_minimum_required(VERSION 2.8)

project(nng-2017-sports-betting)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(Boost)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(sports-betting-hungarian3 src/main.cpp src/hungarian3_solve.cpp src/Hungarian.cpp)
