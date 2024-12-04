cmake_minimum_required(VERSION 3.29)
project(LL1Parser)

set(CMAKE_CXX_STANDARD 17)

add_executable(LL1Parser main.cpp
        LL1Parser.cpp
        LL1Parser.h
        print.cpp
        print.h
        read.cpp
        read.h)
