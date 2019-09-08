cmake_minimum_required(VERSION 3.14)
project(CHIP8 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Setting debug compile definition to true")
    add_compile_definitions(DEBUG=1)
    set(raylib_VERBOSE ON)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS) # Stupid Windows

#set(raylib_STATIC)
find_package(raylib "2.5.0" CONFIG REQUIRED)

include_directories(${raylib_INCLUDE_DIRS})

add_executable(CHIP8 src/main.c src/chip8.c src/rom.c src/log.h src/log.c)
target_link_libraries(CHIP8 ${raylib_LIBRARIES})