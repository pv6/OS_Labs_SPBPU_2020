project("DirectoryCleaner")

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CSS_COMPILER g++)

add_executable(${PROJECT_NAME}
    src/main.cpp src/DirectoryCleaner.cpp)
