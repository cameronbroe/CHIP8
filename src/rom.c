//
// Created by cameron on 9/6/2019.
//

#include "rom.h"

void load_chip8_rom(char* filename, uint8_t* memory_location) {
    FILE* rom_file_handler = fopen(filename, "rb");
    if(rom_file_handler) {
        // Move file cursor to end
        fseek(rom_file_handler, 0, SEEK_END);
        // Use ftell to get number of bytes in file
        size_t file_size = ftell(rom_file_handler);
        // Move file cursor back to beginning
        fseek(rom_file_handler, 0, SEEK_SET);
        size_t bytes_read = fread(memory_location, sizeof(uint8_t), file_size, rom_file_handler);
        // Close our file handler
        fclose(rom_file_handler);
#if DEBUG
        printf("%s is %zu bytes in length and %zu bytes were read\n", filename, file_size, bytes_read);
#endif
    } else {
        fprintf(stderr, "Could not open file: %s\n", filename);
    }
}