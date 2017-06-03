cmake_minimum_required(VERSION 2.8)

project(UTILITY)
include_directories(${UTILITY_DIR})
#include_directories(header)
include_directories(${HUFFMAN_LIB_SOURCE_DIR}/header)

add_executable(utility
               main.cpp)

install(TARGETS utility DESTINATION bin)
target_link_libraries(utility lib)
