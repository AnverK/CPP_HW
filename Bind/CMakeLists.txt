cmake_minimum_required(VERSION 2.8)

project(BIND)

include_directories(${BIND_SOURCE_DIR})

add_executable(bind
    main.cpp
    bind.h
    gtest/gtest-all.cc
    gtest/gtest.h
    gtest/gtest_main.cc
    )
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -O2 -std=c++17")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -std=c++1z -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fno-sanitize-recover -fstack-protector")
endif()

target_link_libraries(bind -lpthread)
