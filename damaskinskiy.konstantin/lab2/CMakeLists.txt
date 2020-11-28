project("Forecast")

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++) 

if (CMAKE_BUILD_TYPE STREQUAL RELEASE)
	add_compile_options(-Wall -Werror)
endif()

foreach(conn shm mq fifo)
    add_executable(${PROJECT_NAME}_host_${conn}
        src/host.cpp src/con_${conn}.cpp src/semaphore.cpp)
    add_executable(${PROJECT_NAME}_client_${conn}
        src/client.cpp src/predictor.cpp src/con_${conn}.cpp src/semaphore.cpp)

    target_link_libraries(${PROJECT_NAME}_host_${conn} rt pthread)
    target_link_libraries(${PROJECT_NAME}_client_${conn} rt pthread)
endforeach()
