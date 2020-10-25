project("DiskMonitor")

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++) 

if (CMAKE_BUILD_TYPE STREQUAL RELEASE)
	add_compile_options(-Wall -Werror)
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp src/config.cpp src/diskmonitor.cpp)

