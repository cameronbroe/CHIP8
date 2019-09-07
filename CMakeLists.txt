cmake_minimum_required(VERSION 3.14)
project(CHIP8 C)

set(CMAKE_C_STANDARD 11)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Setting debug compile definition to true")
    add_compile_definitions(DEBUG=1)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS) # Stupid Windows

add_executable(CHIP8 src/main.c src/chip8.c src/rom.c)