cmake_minimum_required(VERSION 3.28)
project(simulator)

set(CMAKE_CXX_STANDARD 23)

add_library(simulator STATIC Simulator.cpp
        CPU.cpp
        CPU.h
        Registers.cpp
        Registers.h
        Memory.cpp
        Memory.h
        Opcodes.h
        CPUUtil.h
        CPUUtil.cpp)
