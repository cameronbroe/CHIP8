//
// Created by cameron on 9/6/2019.
//

#ifndef CHIP8_ROM_H
#define CHIP8_ROM_H

#ifdef _WIN64
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif

#include <stdio.h>

void load_chip8_rom(char* filename, uint8_t* memory_location);

#endif //CHIP8_ROM_H
