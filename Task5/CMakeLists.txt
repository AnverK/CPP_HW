cmake_minimum_required(VERSION 2.8)

project(PERSISTENT_SET)

include_directories(${PERSISTENT_SET_SOURCE_DIR})

add_executable(persistent_set
               main.cpp
               persistent_set.h
               smart_pointers/shared_ptr.h
               smart_pointers/linked_ptr.h)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -O2 -std=c++11")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -std=c++1z -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fno-sanitize-recover -fstack-protector")
endif()
