// D2CMP Codec.cpp: build the DCC palette-entry maps from 256 used-colour flags.
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13170
// Retail: ESI = outA (colour -> slot), EDX = flags[256], stack outB (slot -> colour); ret 4.
// Both outputs are 0x104 bytes: a dword count follows the 256 bytes.
void d2cmp_13170(uint8_t* outA, const uint8_t* flags, uint8_t* outB) {
    int32_t n = 0;
    for (int32_t i = 0; i < 0x100; i++) {
        if (flags[i]) { outA[i] = (uint8_t)n; outB[n] = (uint8_t)i; n++; }
    }
    int32_t full = 0x100;
    for (int k = 0; k < 4; k++) { outA[0x100 + k] = (uint8_t)(full >> (8 * k)); outB[0x100 + k] = (uint8_t)(n >> (8 * k)); }
}
