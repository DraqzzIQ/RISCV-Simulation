cmake_minimum_required(VERSION 3.28)
project(parser)

set(CMAKE_CXX_STANDARD 23)

add_library(parser STATIC Parser.cpp
        ParsingResult.h
        OpCodes.h
        OpCodes.cpp)

add_executable(parser-cli ParserCLI.cpp)

target_link_libraries(parser-cli PRIVATE parser)