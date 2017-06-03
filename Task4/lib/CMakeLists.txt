cmake_minimum_required(VERSION 2.8)

project(HUFFMAN_LIB)
include_directories(${HUFFMAN_LIB_DIR})
include_directories(header)


set(SOURCES
                        compression.cpp
			decompression.cpp
                        binary_tree.cpp)
			
set(HEADERS
                        header/compression.h
                        header/decompression.h
                        header/binary_tree.h
                        header/exceptions.h)

set(TEST
#                    huffman_library_testing
                        huffman_library_testing.cpp
                        gtest/gtest-all.cc
                        gtest/gtest.h
                        gtest/gtest_main.cc)
			
add_library(lib ${SOURCES} ${HEADERS})

install (TARGETS lib DESTINATION lib)

install (FILES ${HEADERS} DESTINATION include/lib)

add_executable(testing  ${SOURCES} ${HEADERS} ${TEST})

target_link_libraries(testing -lpthread)
