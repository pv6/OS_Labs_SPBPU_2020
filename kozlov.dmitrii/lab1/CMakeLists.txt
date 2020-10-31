cmake_minimum_required(VERSION 3.16.3)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1 main.cpp constants.h cfg_parser.cpp cfg_parser.h
               daemon_base.cpp daemon_base.h my_daemon.cpp my_daemon.h
               my_handler.h types.h)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.cfg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
