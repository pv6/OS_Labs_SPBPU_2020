project("Forecast")

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++) 

set(CMAKE_CXX_FLAGS -lpthread)

if (CMAKE_BUILD_TYPE STREQUAL RELEASE)
	add_compile_options(-Wall -Werror)
endif()

add_executable(${PROJECT_NAME}_host
    src/host.cpp src/con_shm.cpp src/con_mq.cpp src/con_fifo.cpp src/semaphore.cpp)

add_executable(${PROJECT_NAME}_client
    src/client.cpp src/con_shm.cpp src/con_mq.cpp src/con_fifo.cpp src/semaphore.cpp)
