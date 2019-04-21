cmake_minimum_required(VERSION 3.13)
project(OMP C)

set(CMAKE_C_STANDARD 99)

add_executable(OMP main.c)

target_link_libraries(OMP m)
SET(CMAKE_C_FLAGS -pthread)