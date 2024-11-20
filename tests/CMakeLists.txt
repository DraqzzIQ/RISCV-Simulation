cmake_minimum_required(VERSION 3.29)
project(tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run ParserTest.cpp)

target_link_libraries(Google_Tests_run parser)

target_link_libraries(Google_Tests_run gtest gtest_main)