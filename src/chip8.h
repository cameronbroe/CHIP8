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
#include <stdlib.h>
#include <time.h>

#include "constants.h"

typedef struct {
    uint16_t index;
    uint16_t program_counter;
    uint8_t memory[MEMORY_SIZE];
    uint8_t registers[REGISTER_COUNT];
    uint16_t stack[STACK_SIZE];
    uint8_t stack_pointer;
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT];
} chip8_cpu;

void process_cycle();
void initialize_font();
void initialize_machine();
uint8_t random_byte();

// Opcode implementations

// 00E0
void op_cls();
// 00EE
void op_ret();
// 1NNN
void op_jp(uint16_t opcode);
// 2NNN
void op_call(uint16_t opcode);
// 3XKK
void op_se_vx(uint16_t opcode);
// 4XKK
void op_sne_vx(uint16_t opcode);
// 5XY0
void op_se_vx_vy(uint16_t opcode);
// 6XKK
void op_ld_vx(uint16_t opcode);
// 7XKK
void op_add_vx(uint16_t opcode);
// 8XY0
void op_ld_vx_vy(uint16_t opcode);
// 8XY1
void op_or_vx_vy(uint16_t opcode);
// 8XY2
void op_and_vx_vy(uint16_t opcode);
// 8XY3
void op_xor_vx_vy(uint16_t opcode);
// 8XY4
void op_add_vx_vy(uint16_t opcode);
// 8XY5
void op_sub_vx_vy(uint16_t opcode);
// 8XY6
void op_shr_vx(uint16_t opcode);
// 8XY7
void op_subn_vx_vy(uint16_t opcode);
// 8XYE
void op_shl_vx(uint16_t opcode);
// 9XY0
void op_sne_vx_vy(uint16_t opcode);
// ANNN
void op_ld_index_addr(uint16_t opcode);
// BNNN
void op_jp_v0_addr(uint16_t opcode);
// CXKK
void op_rnd_vx(uint16_t opcode);
// DXYN
void op_draw_vx_vy(uint16_t opcode);
// EX9E
void op_skp_vx(uint16_t opcode);
// EXA1
void op_sknp_vx(uint16_t opcode);
// FX07
void op_ld_vx_delay(uint16_t opcode);
// FX0A
void op_ld_vx_key(uint16_t opcode);
// FX15
void op_ld_delay_vx(uint16_t opcode);
// FX18
void op_ld_st_vx(uint16_t opcode);
// FX1E
void op_add_index_vx(uint16_t opcode);
// FX29
void op_ld_font_vx(uint16_t opcode);
// FX33
void op_ld_bcd_vx(uint16_t opcode);
// FX55
void op_ld_index_vx(uint16_t opcode);
// FX65
void op_ld_vx_index(uint16_t opcode);

#endif //CHIP8_CHIP8_H
