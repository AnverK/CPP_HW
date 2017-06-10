cmake_minimum_required(VERSION 2.8)

project(Huffman)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -O2 -std=c++11")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -std=c++11 -D_GLIBCXX_DEBUG")
endif()

add_subdirectory(lib)
add_subdirectory(utility)
