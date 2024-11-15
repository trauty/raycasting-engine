cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project("raycasting-engine")

add_executable(
    raycasting-engine 
    src/main.c
    src/defines.h
    src/window_win32.h
    src/window_win32.c
)

set_property(TARGET raycasting-engine PROPERTY C_STANDARD 99)
