//
// Created by cameron on 9/2/2019.
//

// Documentation on CHIP-8 from
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

#include "chip8.h"
#include "constants.h"

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

void initialize_font() {
    memcpy(cpu.memory + FONTSET_START_ADDRESS, fontset, FONTSET_SIZE);
}

void initialize_machine() {
    cpu.program_counter = CHIP8_ROM_START;
    srand(time(NULL));
    memset(cpu.video, 0, VIDEO_WIDTH * VIDEO_HEIGHT);
}

uint8_t random_byte() {
    return rand() % 256u;
}

void process_cycle() {
    uint8_t x, y, n;
    uint16_t addr;

    // Decode get 16-bit opcode from 8-bit memory buffer at program counter
    uint16_t opcode = cpu.memory[cpu.program_counter] << 8u | cpu.memory[cpu.program_counter + 1];

    // Then increment our program counter
    cpu.program_counter += 2;
}

// 00E0 -> Clear display
void op_cls() {
    memset(cpu.video, 0, VIDEO_WIDTH * VIDEO_HEIGHT);
}

// 00EE -> Return from subroutine
void op_ret() {
    cpu.stack_pointer--;
    cpu.program_counter = cpu.stack[cpu.stack_pointer];
}

// 1NNN -> Jump to address
void op_jp(uint16_t opcode) {
    uint16_t address = opcode & 0xFFFu; // Get last 3 bytes
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
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    if(cpu.registers[Vx] == KK) {
        cpu.program_counter += 2;
    }
}

// 4XKK -> Skip next instruction if Vx != KK
void op_sne_vx(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    if(cpu.registers[Vx] != KK) {
        cpu.program_counter += 2;
    }
}

// 5XY0 -> Skip next instruction if Vx == Vy
void op_se_vx_vy(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    if(cpu.registers[Vx] == cpu.registers[Vy]) {
        cpu.program_counter += 2;
    }
}

// 6XKK -> Set Vx = KK
void op_ld_vx(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    cpu.registers[Vx] = KK;
}

// 7XKK -> Set Vx = Vx + KK
void op_add_vx(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t KK = opcode & 0x00FFu; // Lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] + KK;
}

// 8XY0 -> Set Vx = Vy
void op_ld_vx_vy(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vy];
}

// 8XY1 -> Set Vx = Vx | Vy
void op_or_vx_vy(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] | cpu.registers[Vy];
}

// 8XY2 -> Set Vx = Vx & Vy
void op_and_vx_vy(uint16_t opcode) {
    uint8_t Vx = (opcode >> 8u) & 0x0Fu; // Lowest 4 bits of highest byte
    uint8_t Vy = (opcode >> 4u) & 0x0Fu; // Highest 4 bits of lowest byte
    cpu.registers[Vx] = cpu.registers[Vx] & cpu.registers[Vy];
}
