cmake_minimum_required(VERSION 3.4)
project(lab1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
        main.cpp
        parser.cpp
        parser.h Daemon.cpp Daemon.h)

add_executable(${PROJECT_NAME} ${SOURCES})
