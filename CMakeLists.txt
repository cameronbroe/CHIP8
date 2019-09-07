cmake_minimum_required(VERSION 3.14)
project(CHIP8 C)

set(CMAKE_C_STANDARD 11)

add_compile_definitions(DEBUG=1)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS) # Stupid Windows

add_executable(CHIP8 src/main.c src/chip8.c src/chip8.h src/rom.h src/rom.c src/constants.h)