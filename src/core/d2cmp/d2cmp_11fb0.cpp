// 1.13c D2CMP+0x11FB0 (#10020): row (bit index + 1) x column a lookup in a 8x64 u32 table.
// stdcall (a, mask). Out of range, mask 0 or non-single-bit input is fatal in retail (_exit(-1)).
// The probe src/core/fn/d2cmp_11fb0.cpp drives this body.
#include <cstdint>
#include <unistd.h>

// Retail's table is generated-looking data; row r (2..7) is (c + half step) >> (7-r), zeroed at >= 1<<(r-1); rows 0,1 are zero. Checked against all 512 dwords.
static uint32_t tbl_11FB0(uint32_t r, uint32_t c) {
    if (r < 2) return 0;
    uint32_t k = 7 - r, v = (c + ((1u << k) >> 1)) >> k;
    return v < (1u << (r - 1)) ? v : 0;
}

// 1.13c D2CMP+0x11FB0
extern "C" uint32_t d2cmp_11fb0(uint32_t a, uint32_t mask) {
    if (a > 0x3f) _exit(-1);
    if (!mask || (mask & (mask - 1))) _exit(-1);
    uint32_t i = 0;
    while (!((mask >> i) & 1)) i++;
    if (i + 1 > 7) _exit(-1);
    return tbl_11FB0(i + 1, a);
}
