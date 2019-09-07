#include <stdio.h>
#include "chip8.h"
#include "rom.h"
#include "constants.h"

#include <raylib.h>

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

    InitWindow(RAYLIB_WIDTH, RAYLIB_HEIGHT, "CHIP-8");
    SetTargetFPS(60); // 60 frames per second! Helps out with delay and sound timers
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            for(int y = 0; y < VIDEO_HEIGHT; y++) {
                for(int x = 0; x < VIDEO_WIDTH; x++) {
                    uint32_t pixel_state = cpu.video[y * VIDEO_WIDTH + x];
#ifdef DEBUG
                    printf("Pixel state at (%d, %d) = %.8x\n", x, y, pixel_state);
#endif
                    if(pixel_state) {
                        DrawRectangle(x * VIDEO_WIDTH_SCALE, y * VIDEO_HEIGHT_SCALE, VIDEO_WIDTH_SCALE,
                                      VIDEO_HEIGHT_SCALE, DARKBLUE);
                    } else {
                        DrawRectangle(x * VIDEO_WIDTH_SCALE, y * VIDEO_HEIGHT_SCALE, VIDEO_WIDTH_SCALE,
                                      VIDEO_HEIGHT_SCALE, RAYWHITE);
                    }
                }
            }
        EndDrawing();
    }
    return 0;
}