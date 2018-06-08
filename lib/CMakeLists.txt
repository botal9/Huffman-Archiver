cmake_minimum_required(VERSION 2.8)
project(huffman_lib)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_library(huffman_lib STATIC
        huffman_tree.h
        huffman_tree.cpp
        archiver.h
        archiver.cpp
        )

#,undefined

