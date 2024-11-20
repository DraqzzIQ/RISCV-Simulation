cmake_minimum_required(VERSION 3.28)
project(parser)

set(CMAKE_CXX_STANDARD 23)

add_library(parser STATIC Parser.cpp
        ParsingResult.h)

add_executable(cli ParserCLI.cpp)

target_link_libraries(cli PRIVATE parser)