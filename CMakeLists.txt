cmake_minimum_required(VERSION 2.8)
project(huffman)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/util)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_subdirectory(lib)
add_subdirectory(lib/test)
add_subdirectory(util)

add_executable(huffman util/Run.cpp)
target_link_libraries(huffman huffman_lib)

