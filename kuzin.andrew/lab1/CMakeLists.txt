cmake_minimum_required(VERSION 3.15)
project(manager)

set(CMAKE_CXX_STANDARD 17)

add_executable(manager main.cpp Manager.cpp Manager.h)