cmake_minimum_required(VERSION 3.0.0)
project(lab_1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER g++)

set(SOURCE_FILES src/main.cpp 
        src/FileTransfer.cpp include/FileTransfer.h
        src/ConfigParser.cpp include/ConfigParser.h
        src/DaemonManager.cpp include/DaemonManager.h)

add_executable(lab_1 ${SOURCE_FILES})