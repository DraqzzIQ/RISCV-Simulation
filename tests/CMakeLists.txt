cmake_minimum_required(VERSION 3.28)
project(tests)
add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 23)

add_executable(Google_Tests_run
        ParserTest.cpp
        CPUTest.cpp)

target_link_libraries(Google_Tests_run parser emulator)

target_link_libraries(Google_Tests_run gtest gtest_main)