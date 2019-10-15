#include <stdio.h>
#include "chip8.h"
#include "rom.h"
#include "constants.h"
#include "log.h"
#include "timer.h"

#include <raylib.h>
#include <sys/time.h>

extern chip8_cpu cpu;
int multiplier = 1;

void increment_multiplier() {
    if(multiplier < 10) {
        multiplier++;
        printf("incrementing multiplier\n");
        SetTargetFPS(60 * multiplier);
    }
}

void decrement_multiplier() {
    if(multiplier > 1) {
        multiplier--;
        printf("decrementing multiplier\n");
        SetTargetFPS(60 * multiplier);
    }
}

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "Missing argument for what file to load\n");
        return 1;
    }
    initialize_machine();
    initialize_font();
    initialize_logger();

    start_delay_timer();
    start_sound_timer();

    char* filename = argv[1];
    load_chip8_rom(filename, cpu.memory + CHIP8_ROM_START);

    InitWindow(RAYLIB_WIDTH, RAYLIB_HEIGHT, "CHIP-8");
    SetTargetFPS(60 * multiplier); // 60 frames per second! Helps out with delay and sound timers
    printf("%d\n", CLOCKS_PER_SEC);
    while(!WindowShouldClose()) {
        process_cycle();
        // Keymaps
        if(IsKeyDown(KEY_ONE)) { cpu.keypad[0x1] = 1; }
        if(IsKeyDown(KEY_TWO)) { cpu.keypad[0x2] = 1; }
        if(IsKeyDown(KEY_THREE)) { cpu.keypad[0x3] = 1; }
        if(IsKeyDown(KEY_FOUR)) { cpu.keypad[0xC] = 1; }
        if(IsKeyDown(KEY_Q)) { cpu.keypad[0x4] = 1; }
        if(IsKeyDown(KEY_W)) { cpu.keypad[0x5] = 1; }
        if(IsKeyDown(KEY_E)) { cpu.keypad[0x6] = 1; }
        if(IsKeyDown(KEY_R)) { cpu.keypad[0xD] = 1; }
        if(IsKeyDown(KEY_A)) { cpu.keypad[0x7] = 1; }
        if(IsKeyDown(KEY_S)) { cpu.keypad[0x8] = 1; }
        if(IsKeyDown(KEY_D)) { cpu.keypad[0x9] = 1; }
        if(IsKeyDown(KEY_F)) { cpu.keypad[0xE] = 1; }
        if(IsKeyDown(KEY_Z)) { cpu.keypad[0xA] = 1; }
        if(IsKeyDown(KEY_X)) { cpu.keypad[0x0] = 1; }
        if(IsKeyDown(KEY_C)) { cpu.keypad[0xB] = 1; }
        if(IsKeyDown(KEY_V)) { cpu.keypad[0xF] = 1; }

        if(IsKeyUp(KEY_ONE)) { cpu.keypad[0x1] = 0; }
        if(IsKeyUp(KEY_TWO)) { cpu.keypad[0x2] = 0; }
        if(IsKeyUp(KEY_THREE)) { cpu.keypad[0x3] = 0; }
        if(IsKeyUp(KEY_FOUR)) { cpu.keypad[0xC] = 0; }
        if(IsKeyUp(KEY_Q)) { cpu.keypad[0x4] = 0; }
        if(IsKeyUp(KEY_W)) { cpu.keypad[0x5] = 0; }
        if(IsKeyUp(KEY_E)) { cpu.keypad[0x6] = 0; }
        if(IsKeyUp(KEY_R)) { cpu.keypad[0xD] = 0; }
        if(IsKeyUp(KEY_A)) { cpu.keypad[0x7] = 0; }
        if(IsKeyUp(KEY_S)) { cpu.keypad[0x8] = 0; }
        if(IsKeyUp(KEY_D)) { cpu.keypad[0x9] = 0; }
        if(IsKeyUp(KEY_F)) { cpu.keypad[0xE] = 0; }
        if(IsKeyUp(KEY_Z)) { cpu.keypad[0xA] = 0; }
        if(IsKeyUp(KEY_X)) { cpu.keypad[0x0] = 0; }
        if(IsKeyUp(KEY_C)) { cpu.keypad[0xB] = 0; }
        if(IsKeyUp(KEY_V)) { cpu.keypad[0xF] = 0; }

        if(IsKeyPressed(KEY_RIGHT_BRACKET)) { increment_multiplier(); }
        if(IsKeyPressed(KEY_LEFT_BRACKET)) { decrement_multiplier(); }

        BeginDrawing();
            ClearBackground(BLACK);
            for(int y = 0; y < VIDEO_HEIGHT; y++) {
                for(int x = 0; x < VIDEO_WIDTH; x++) {
                    uint32_t pixel_state = cpu.video[y * VIDEO_WIDTH + x];
#ifdef DEBUG
                    //log_debug("Pixel state at (%d, %d) = %.8x\n", x, y, pixel_state);
#endif
                    if(pixel_state) {
                        DrawRectangle(x * VIDEO_WIDTH_SCALE, y * VIDEO_HEIGHT_SCALE, VIDEO_WIDTH_SCALE,
                                      VIDEO_HEIGHT_SCALE, WHITE);
                    } else {
                        DrawRectangle(x * VIDEO_WIDTH_SCALE, y * VIDEO_HEIGHT_SCALE, VIDEO_WIDTH_SCALE,
                                      VIDEO_HEIGHT_SCALE, BLACK);
                    }
                }
            }
#ifdef DEBUG
            DrawFPS(0, 20);
#endif
        EndDrawing();
    }
    stop_delay_timer();
    stop_sound_timer();
    close_logger();
    return 0;
}