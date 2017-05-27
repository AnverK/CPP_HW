cmake_minimum_required(VERSION 2.8)

project(Utility)
include_directories(${Utility_DIR})
include_directories(${Huffman_library_SOURCE_DIR})

add_executable(utility
               main.cpp)

install(TARGETS utility DESTINATION bin)
target_link_libraries(utility library)
