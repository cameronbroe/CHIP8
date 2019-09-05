#include <stdio.h>
#include "chip8.h"

int main() {
    initialize_font();

    uint8_t data_sep[2] = { 0xFE, 0xA0 };
    printf("data[0]: %.2x", data_sep[0]);
    printf("\n");
    printf("data[0] shifted 8 left: %.4x", data_sep[0] << 8);
    printf("\n");
    printf("data[1]: %.2x", data_sep[1]);
    printf("\n");
    uint16_t data = data_sep[0] << 8 | data_sep[0 + 1];
    // 0x00FFFF00
    uint16_t shifted_8 = data >> 8;
    uint16_t shifted_4 = data >> 4;
    printf("data combined: %.4x", data);
    printf("\n");
    printf("shifted 8 right: %.4x", shifted_8);
    printf("\n");
    printf("shifted 4 right: %.4x", shifted_4);
    printf("\n");
    printf("shifted 8 right, lowest 4 bits: %.4x", shifted_8 & 0x000F);
    printf("\n");
    printf("shifted 4 right, highest 4 bits: %.4x", shifted_4 & 0x000F);
    return 0;
}