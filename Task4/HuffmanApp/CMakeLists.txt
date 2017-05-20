cmake_minimum_required(VERSION 2.8)

project(HuffmanApp)

include_directories(${HuffmanApp_SOURCE_DIR})

add_executable(HuffmanApp
               huffman.cpp
               huffman.h
               main.cpp
               )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(HuffmanApp -lpthread)
