cmake_minimum_required(VERSION 2.8)

project(nng-2017-lego-wall)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CACHE_STATS "Enable cache stats" OFF)
if(CACHE_STATS)
    add_definitions(-DCACHE_STATS)
endif()

find_package(Boost)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -O3")

add_executable(lego main.cpp)
