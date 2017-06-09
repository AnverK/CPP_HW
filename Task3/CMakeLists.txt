cmake_minimum_required(VERSION 2.8)

project(BIGINT)

include_directories(${BIGINT_SOURCE_DIR})
include_directories(vector)

add_executable(main
               main.cpp
               big_integer.h
               big_integer.cpp
               vector/vector_uint32.cpp
               vector/vector_uint32.h
               )

add_executable(big_integer_testing
               big_integer_testing.cpp
               big_integer.h
               big_integer.cpp
               vector/vector_uint32.cpp
               vector/vector_uint32.h
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc
                )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++11 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(big_integer_testing -lpthread)
