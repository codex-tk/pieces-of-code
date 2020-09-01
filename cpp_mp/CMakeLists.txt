cmake_minimum_required(VERSION 3.0.0)

project(cpp_mp VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (UNIX AND NOT APPLE)
add_definitions( -Wall -std=c++17 -o2 -g -ggdb -fconcepts -DSOCKLEN_T=socklen_t )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

add_executable(cpp_mp main.cpp)