cmake_minimum_required(VERSION 2.8)

project(MY_LIST)

include_directories(${MY_LIST_SOURCE_DIR})

add_executable(my_list
               main.cpp
               my_list.h
               my_list.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -O2 -std=c++11")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -std=c++11 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector")
endif()

#target_link_libraries(big_integer_testing -lpthread)
