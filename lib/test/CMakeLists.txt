cmake_minimum_required(VERSION 2.8)
project(huffman_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(huffman_test
        tests.cpp
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc
        )

target_link_libraries(huffman_test huffman_lib -lpthread)



