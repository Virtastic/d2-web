// 1.13c D2CMP+0x9CC0 (export #10084, 112 bytes): farthest palette entry (squared RGB distance).
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09CC0
// Mirror of 0x9D30 with the comparison reversed: best starts at 0, strictly greater wins (first
// maximum). count==0 returns 0; otherwise count's bits 8+ with the index in the low byte.
extern "C" uint32_t d2cmp_9cc0(const uint8_t* pal, uint32_t count, int32_t p3, int32_t p4, int32_t p5) {
    if (count == 0) return 0;
    uint32_t best = 0, idx = 0;
    for (uint32_t i = 0; i < count; ++i, pal += 4) {
        uint32_t a = (uint32_t)pal[0] - (uint32_t)p3, b = (uint32_t)pal[1] - (uint32_t)p4, c = (uint32_t)pal[2] - (uint32_t)p5;
        uint32_t d = c * c + b * b + a * a;
        if (best < d) { best = d; idx = i; }
    }
    return (count & ~0xffu) | (idx & 0xffu);
}
