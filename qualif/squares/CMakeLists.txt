cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DEBUG_MEMORY "Enable memory profiling" OFF)
if(DEBUG_MEMORY)
    add_definitions(-DDEBUG_MEMORY)
endif()

add_executable(squares
    squares.cpp
)
