cmake_minimum_required(VERSION 3.28)
project(emulator)

set(CMAKE_CXX_STANDARD 23)

add_library(emulator STATIC Emulator.cpp
        CPU.cpp
        CPU.h
        Registers.cpp
        Registers.h
        Memory.cpp
        Memory.h
        Opcodes.h)
