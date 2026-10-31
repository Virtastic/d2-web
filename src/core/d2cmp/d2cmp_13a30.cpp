// D2CMP Codec.cpp: unpack the DCC 256-bit colour-use mask and build the entry maps.
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13A30
// Retail: stdcall (bits256, outA, outB), ret 0xC; bit i = bits[i>>5] & gdwBitMasks[i&31] (Fog, 1<<i).
void d2cmp_13a30(const uint32_t* bits256, uint8_t* outA, uint8_t* outB) {
    uint8_t flags[0x100];
    for (int32_t i = 0; i < 0x100; i++) flags[i] = (bits256[i >> 5] & (1u << (i & 31))) != 0;
    d2cmp_13170(outA, flags, outB);
}
