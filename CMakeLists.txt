cmake_minimum_required(VERSION 3.27)
project(CPUSimulator)

set(CMAKE_CXX_STANDARD 26)

include_directories(include)

add_subdirectory(source)
add_subdirectory(test)