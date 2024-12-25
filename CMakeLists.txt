cmake_minimum_required(VERSION 3.12)
project(hw4p2 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif(CMAKE_BUILD_TPYE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-pg -g")
endif()

add_executable(bignum main.cpp bignum.cpp)

