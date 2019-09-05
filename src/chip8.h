//
// Created by cameron on 9/2/2019.
//

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#ifdef _WIN64
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif

#include <string.h>

typedef struct {
    uint8_t v0;
    uint8_t v1;
    uint8_t v2;
    uint8_t v3;
    uint8_t v4;
    uint8_t v5;
    uint8_t v6;
    uint8_t v7;
    uint8_t v8;
    uint8_t v9;
    uint8_t vA;
    uint8_t vB;
    uint8_t vC;
    uint8_t vD;
    uint8_t vE;
    uint8_t vF; // flags

    uint16_t index;
    uint16_t program_counter;
} chip8_registers;

void process_cycle();
void initialize_font();

#endif //CHIP8_CHIP8_H
