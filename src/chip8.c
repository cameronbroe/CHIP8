//
// Created by cameron on 9/2/2019.
//

// Documentation on CHIP-8 from
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
// Also, used this article to help with some implementation details and inspiration
// https://austinmorlan.com/posts/chip8_emulator/

#include "chip8.h"
#include "constants.h"

#include "log.h"

#ifdef DEBUG
#include <stdio.h>
#include <sys/time.h>
#endif

typedef void (*opcode_func)(uint16_t);

chip8_cpu cpu;

uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Call tables for opcodes
opcode_func table[0xF + 1];
opcode_func table0[0xE + 1];
opcode_func table8[0xE + 1];
opcode_func tableE[0xE + 1];
opcode_func tableF[0x65 + 1];

void initialize_font() {
    memcpy(cpu.memory + FONTSET_START_ADDRESS, fontset, FONTSET_SIZE);
}

void initialize_machine() {
    cpu.program_counter = CHIP8_ROM_START;
    srand((uint32_t) time(NULL));
    memset(cpu.video, 0, VIDEO_WIDTH * VIDEO_HEIGHT);

    // Load up call tables
    table[0x0] = &call_table0;
    table[0x1] = &op_jp;
    table[0x2] = &op_call;
    table[0x3] = &op_se_vx;
    table[0x4] = &op_sne_vx;
    table[0x5] = &op_se_vx_vy;
    table[0x6] = &op_ld_vx;
    table[0x7] = &op_add_vx;
    table[0x8] = &call_table8;
    table[0x9] = &op_sne_vx_vy;
    table[0xA] = &op_ld_index_addr;
    table[0xB] = &op_jp_v0_addr;
    table[0xC] = &op_rnd_vx;
    table[0xD] = &op_draw_vx_vy;
    table[0xE] = &call_tableE;
    table[0xF] = &call_tableF;

    table0[0x0] = &op_cls;
    table0[0xE] = &op_ret;

    table8[0x0] = &op_ld_vx_vy;
    table8[0x1] = &op_or_vx_vy;
    table8[0x2] = &op_and_vx_vy;
    table8[0x3] = &op_xor_vx_vy;
    table8[0x4] = &op_add_vx_vy;
    table8[0x5] = &op_sub_vx_vy;
    table8[0x7] = &op_subn_vx_vy;
    table8[0xE] = &op_shl_vx;

    tableE[0x1] = &op_sknp_vx;
    tableE[0xE] = &op_skp_vx;

    tableF[0x07] = &op_ld_vx_delay;
    tableF[0x0A] = &op_ld_vx_key;
    tableF[0x15] = &op_ld_delay_vx;
    tableF[0x18] = &op_ld_st_vx;
    tableF[0x1E] = &op_add_index_vx;
    tableF[0x29] = &op_ld_font_vx;
    tableF[0x33] = &op_ld_bcd_vx;
    tableF[0x55] = &op_ld_index_vx;
    tableF[0x65] = &op_ld_vx_index;
}

void call_table0(uint16_t opcode) {
    (*(table0[opcode & 0xFu]))(opcode);
}

void call_table8(uint16_t opcode) {
    (*(table8[opcode & 0xFu]))(opcode);
}

void call_tableE(uint16_t opcode) {
    (*(tableE[opcode & 0xFu]))(opcode);
}

void call_tableF(uint16_t opcode) {
    (*(tableF[opcode & 0xFFu]))(opcode);
}

uint8_t random_byte() {
    return rand() % 256u;
}

void process_cycle() {
    log_debug("processed cycle at: %d\n", clock());
    // Decode get 16-bit opcode from 8-bit memory buffer at program counter
    uint16_t opcode = cpu.memory[cpu.program_counter] << 8u | cpu.memory[cpu.program_counter + 1];

#ifdef DEBUG
    log_debug("program_counter: %04x\n", cpu.program_counter);
    log_debug("opcode: %04x\n", opcode);
#endif

    // Then increment our program counter
    cpu.program_counter += 2;

    // Process decoded opcode
    (*(table[opcode >> 12u]))(opcode);
}

void tick_sound_timer() {
    if(cpu.sound_timer > 0) {
#ifdef DEBUG
        struct timeval now;
        gettimeofday(&now, NULL);
        double time = (now.tv_sec * 1000.0) + (now.tv_usec / 1000.0);
        log_debug("[%f] ticking sound_timer: %d\n", time, cpu.sound_timer);
#endif
        cpu.sound_timer--;
    }
}

void tick_delay_timer() {
    if(cpu.delay_timer > 0) {
#ifdef DEBUG
        struct timeval now;
        gettimeofday(&now, NULL);
        double time = (now.tv_sec * 1000.0) + (now.tv_usec / 1000.0);
        log_debug("[%f] ticking delay_timer: %d\n", time, cpu.delay_timer);
#endif
        cpu.delay_timer--;
    }
}

// 00E0 -> Clear display
void op_cls(uint16_t opcode) {
    memset(cpu.video, 0, VIDEO_WIDTH * VIDEO_HEIGHT);
    log_debug("clearing screen\n");
}

// 00EE -> Return from subroutine
void op_ret(uint16_t opcode) {
    log_debug("stack pointer before ret: %d\n", cpu.stack_pointer);
    cpu.stack_pointer--;
    cpu.program_counter = cpu.stack[cpu.stack_pointer];
    log_debug("program counter from stack: %04x\n", cpu.program_counter);
    log_debug("stack pointer after ret: %d\n", cpu.stack_pointer);
}

// 1NNN -> Jump to address
void op_jp(uint16_t opcode) {
    uint16_t address = opcode & 0xFFFu; // Get last 3 bytes
    log_debug("setting PC to addr: %04x\n", address);
    cpu.program_counter = address;
}

// 2NNN -> Call subroutine at address
void op_call(uint16_t opcode) {
    uint16_t address = opcode & 0xFFFu; // Get last 3 bytes
    cpu.stack[cpu.stack_pointer] = cpu.program_counter;
    cpu.stack_pointer++;
    cpu.program_counter = address;
}

// 3XKK -> Skip instruction if Vx = KK
void op_se_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    if(cpu.registers[Vx] == KK) {
        cpu.program_counter += 2;
    }
}

// 4XKK -> Skip next instruction if Vx != KK
void op_sne_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    if(cpu.registers[Vx] != KK) {
        cpu.program_counter += 2;
    }
}

// 5XY0 -> Skip next instruction if Vx == Vy
void op_se_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    if(cpu.registers[Vx] == cpu.registers[Vy]) {
        cpu.program_counter += 2;
    }
}

// 6XKK -> Set Vx = KK
void op_ld_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    cpu.registers[Vx] = KK;
}

// 7XKK -> Set Vx = Vx + KK
void op_add_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] + KK;
}

// 8XY0 -> Set Vx = Vy
void op_ld_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vy];
}

// 8XY1 -> Set Vx = Vx | Vy
void op_or_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] | cpu.registers[Vy];
}

// 8XY2 -> Set Vx = Vx & Vy
void op_and_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] & cpu.registers[Vy];
}

// 8XY3 -> Set Vx = Vx ^ Vy
void op_xor_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] ^ cpu.registers[Vy];
}

// 8XY4 -> Set Vx = Vx + Vy, set VF = carry.
void op_add_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    uint16_t result = cpu.registers[Vx] + cpu.registers[Vy];
    if(result > 255u) {
        cpu.registers[0xF] = 1;
    } else {
        cpu.registers[0xF] = 0;
    }
    cpu.registers[Vx] = result & 0x00FFu;
}

// 8XY5 -> Set Vx = Vx - Vy, set VF = NOT borrow.
void op_sub_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    if(cpu.registers[Vx] > cpu.registers[Vy]) {
        cpu.registers[0xF] = 1;
    } else {
        cpu.registers[0xF] = 0;
    }
    cpu.registers[Vx] = cpu.registers[Vx] - cpu.registers[Vy];
}

// 8XY6 -> Set Vx = Vx >> 1.
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void op_shr_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t check_lsb = cpu.registers[Vx] & 0x1u;
    if(check_lsb == 1) {
        cpu.registers[0xF] = 1;
    } else {
        cpu.registers[0xF] = 0;
    }
    cpu.registers[Vx] = cpu.registers[Vx] >> 1u;
}

// 8XY7 -> Set Vx = Vy - Vx, set VF = NOT borrow.
void op_subn_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    if(cpu.registers[Vy] > cpu.registers[Vx]) {
        cpu.registers[0xF] = 1;
    } else {
        cpu.registers[0xF] = 0;
    }
    cpu.registers[Vx] = cpu.registers[Vy] - cpu.registers[Vx];
}

// 8XYE -> Set Vx = Vx << 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is multiplied by 2.
void op_shl_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t check_msb = (uint8_t) (cpu.registers[Vx] >> 7u) & 0x1u;
    if(check_msb == 1) {
        cpu.registers[0xF] = 1;
    } else {
        cpu.registers[0xF] = 0;
    }
    cpu.registers[Vx] = cpu.registers[Vx] << 1u;
}

// 9XY0 -> Skip next instruction if Vx != Vy.
void op_sne_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    if(cpu.registers[Vx] != cpu.registers[Vy]) {
        cpu.program_counter += 2;
    }
}

// ANNN -> Set index = NNN.
void op_ld_index_addr(uint16_t opcode) {
    uint16_t address = opcode & 0xFFFu;
    log_debug("setting I to addr: %04x\n", address);
    cpu.index = address;
}

// BNNN -> Jump to location NNN + V0.
void op_jp_v0_addr(uint16_t opcode) {
    uint16_t address = opcode & 0xFFFu;
    cpu.program_counter = address + cpu.registers[0x0];
}

// CXKK -> Set Vx = random byte AND kk.
void op_rnd_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    uint8_t rbyte = random_byte();
    cpu.registers[Vx] = rbyte & KK;
}

// DXYN -> Display n-byte sprite starting at memory location index at (Vx, Vy), set VF = collision.
void op_draw_vx_vy(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (uint8_t) (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    uint8_t height = opcode & 0x000Fu;

    log_debug("starting draw at (%d, %d)\n", cpu.registers[Vx], cpu.registers[Vy]);

    // Using modulus to support wrap around
    uint8_t xPosition = cpu.registers[Vx] % VIDEO_WIDTH;
    uint8_t yPosition = cpu.registers[Vy] % VIDEO_HEIGHT;

    log_debug("wrapped position is (%d, %d)\n", xPosition, yPosition);

    cpu.registers[0xF] = 0;

    for(uint32_t row = 0; row < height; row++) {
        uint8_t spriteByte = cpu.memory[cpu.index + row];

        for(uint32_t column = 0; column < SPRITE_WIDTH; column++) {
            uint8_t spritePixel = spriteByte & (0b10000000 >> column);
            uint32_t *screenPixel = &cpu.video[(yPosition + row) * VIDEO_WIDTH + (xPosition + column)];

            // If sprite pixel is on
            if(spritePixel) {
                // If screen pixel is already on
                if(*screenPixel == SCREEN_PIXEL_ON) {
                    cpu.registers[0xF] = 1;
                }

                *screenPixel ^= SCREEN_PIXEL_ON;
            }
        }
    }
}

// EX9E -> Skip next instruction if key with the value of Vx is pressed.
void op_skp_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t key = cpu.registers[Vx];

    if(cpu.keypad[key]) {
        cpu.program_counter += 2;
    }
}

// EXA1 -> Skip next instruction if key with the value of Vx is not pressed.
void op_sknp_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t key = cpu.registers[Vx];

    if(!cpu.keypad[key]) {
        cpu.program_counter += 2;
    }
}

// FX07 -> Set Vx = delay timer value.
void op_ld_vx_delay(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    cpu.registers[Vx] = cpu.delay_timer;
}

// FX0A -> Wait for a key press, store the value of the key in Vx.
void op_ld_vx_key(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    if(cpu.keypad[0x0]) {
        cpu.registers[Vx] = 0x0;
    } else if(cpu.keypad[0x1]) {
        cpu.registers[Vx] = 0x1;
    } else if(cpu.keypad[0x2]) {
        cpu.registers[Vx] = 0x2;
    } else if(cpu.keypad[0x3]) {
        cpu.registers[Vx] = 0x3;
    } else if(cpu.keypad[0x4]) {
        cpu.registers[Vx] = 0x4;
    } else if(cpu.keypad[0x5]) {
        cpu.registers[Vx] = 0x5;
    } else if(cpu.keypad[0x6]) {
        cpu.registers[Vx] = 0x6;
    } else if(cpu.keypad[0x7]) {
        cpu.registers[Vx] = 0x7;
    } else if(cpu.keypad[0x8]) {
        cpu.registers[Vx] = 0x8;
    } else if(cpu.keypad[0x9]) {
        cpu.registers[Vx] = 0x9;
    } else if(cpu.keypad[0xA]) {
        cpu.registers[Vx] = 0xA;
    } else if(cpu.keypad[0xB]) {
        cpu.registers[Vx] = 0xB;
    } else if(cpu.keypad[0xC]) {
        cpu.registers[Vx] = 0xC;
    } else if(cpu.keypad[0xD]) {
        cpu.registers[Vx] = 0xD;
    } else if(cpu.keypad[0xE]) {
        cpu.registers[Vx] = 0xE;
    } else if(cpu.keypad[0xF]) {
        cpu.registers[Vx] = 0xF;
    } else {
        // Go back an instruction
        cpu.program_counter -= 2;
    }
}

// FX15 -> Set delay timer = Vx.
void op_ld_delay_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    cpu.delay_timer = cpu.registers[Vx];
}

// FX18 -> Set sound timer = Vx.
void op_ld_st_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    cpu.sound_timer = cpu.registers[Vx];
}

// FX1E -> Set I = I + Vx.
void op_add_index_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    cpu.index = cpu.index + cpu.registers[Vx];
}

// FX29 -> Set I = location of sprite for digit Vx.
void op_ld_font_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t digit = cpu.registers[Vx];
    cpu.index = FONTSET_START_ADDRESS + (digit * FONT_CHARACTER_SIZE);
}

// FX33 -> Store BCD representation of Vx in memory locations I, I+1, and I+2.
void op_ld_bcd_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t value = cpu.registers[Vx];

    // Insert ones-place
    cpu.memory[cpu.index + 2] = value % 10;
    value = value / 10;

    // Insert tens-place
    cpu.memory[cpu.index + 1] = value % 10;
    value = value / 10;

    // Insert hundreds-place
    cpu.memory[cpu.index] = value % 10;
}

// FX55 -> Store registers V0 through Vx in memory starting at location I.
void op_ld_index_vx(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte

    // Loop through registers up to Vx and store their values in memory
    for(int i = 0; i < Vx; i++) {
        cpu.memory[cpu.index + i] = cpu.registers[i];
    }
}

// FX65 -> Read registers V0 through Vx from memory starting at location I.
void op_ld_vx_index(uint16_t opcode) {
    uint8_t Vx = (uint8_t) (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte

    // Loop through registers up to Vx and read value at I + Vi into Vi
    for(int i = 0; i < Vx; i++) {
        cpu.registers[i] = cpu.memory[cpu.index + i];
    }
}