cmake_minimum_required(VERSION 3.15)
project(_lab1)

set(CMAKE_CXX_STANDARD 14)

add_executable(_lab1 main.cpp parser.cpp parser.h daemon.cpp daemon.h)
