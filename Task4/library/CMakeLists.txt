cmake_minimum_required(VERSION 2.8)

project(Huffman_library)
include_directories(${Huffman_library_DIR})
include_directories(header)

set(SOURCES
			compression.cpp
			decompression.cpp
			binary_tree.cpp)
			
set(HEADERS
			compression.h
			decompression.h
			binary_tree.h)
			
add_library(library ${SOURCES} ${HEADERS})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic -O2 ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector")
endif()
		
install (TARGETS library DESTINATION library)

install (FILES ${HEADERS} DESTINATION include/library)
