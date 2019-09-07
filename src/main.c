#include <stdio.h>
#include "chip8.h"
#include "rom.h"
#include "constants.h"

extern chip8_cpu cpu;

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "Missing argument for what file to load\n");
        return 1;
    }
    initialize_machine();
    initialize_font();

    char* filename = argv[1];
    load_chip8_rom(filename, cpu.memory + CHIP8_ROM_START);
    return 0;
}