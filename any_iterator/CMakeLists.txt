cmake_minimum_required(VERSION 2.8)

project(any_iterator)

include_directories(${ANY_ITERATOR_SOURCE_DIR})

add_executable(any_iterator
    main.cpp
    any_iterator.h
    )
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -std=c++17")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -std=c++17 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address")
endif()

target_link_libraries(any_iterator -lpthread -lgtest)
