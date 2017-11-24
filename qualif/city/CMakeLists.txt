cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable(city
    src/main2.cpp
)

target_compile_definitions(city PRIVATE
    LOCAL_BUILD
)

add_executable(city0
    src/main.cpp
)
