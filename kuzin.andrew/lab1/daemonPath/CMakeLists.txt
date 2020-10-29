cmake_minimum_required(VERSION 3.16)
project(daemon)

set(CMAKE_CXX_STANDARD 17)

add_executable(daemon main.cpp daemon.cpp daemon.h DaemonException.h types.h Event.h)

