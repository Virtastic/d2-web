// 1.13c D2CMP+0xA490 (431 bytes): the two-thirds table at block+0x6B500 and the max-alpha blend at +0x5B500.
#include "d2cmp_palstate.h"
// Retail's reciprocal-multiply step for a nonzero byte: v + ((hi32(v*0x55555555) - v) >> 1, rounded
// toward zero), i.e. about 2v/3 (3 -> 3, 255 -> 171); zero stays zero.
static uint8_t twoThirds(uint8_t v) {
    if (v == 0) return 0;
    int32_t t = (int32_t)(((int64_t)v * 0x55555555) >> 32) - (int32_t)v;
    t >>= 1;
    t += (int32_t)((uint32_t)t >> 31);
    return (uint8_t)(v + t);
}
// 1.13c D2CMP+0x0A490
// Part 2: for source entry j (outer) with m = max of its three channels, out[j + i*256] = nearest of
// (d[c]*(255-m) + s[c]*m)/255, d = entry i (inner).
extern "C" void d2cmp_a490(uint8_t* block) {
    for (uint32_t i = 0; i < 0x100; ++i) {
        const uint8_t* p = block + i * 4;
        block[0x6b500 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, twoThirds(p[0]), twoThirds(p[1]), twoThirds(p[2]));
    }
    uint8_t* out = block + 0x5b500;
    for (uint32_t j = 0; j < 0x100; ++j) {
        const uint8_t* s = block + j * 4;
        uint32_t m = s[1];
        if (m < s[0]) m = s[0];
        uint32_t mm = s[2];
        if (s[2] < m) mm = m;
        uint32_t im = 0xff - mm;
        for (uint32_t i = 0; i < 0x100; ++i) {
            const uint8_t* d = block + i * 4;
            out[j + i * 0x100] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)(((d[0] * im + s[0] * mm) / 0xff) & 0xff),
                                                         (int32_t)(((d[1] * im + s[1] * mm) / 0xff) & 0xff),
                                                         (int32_t)(((d[2] * im + s[2] * mm) / 0xff) & 0xff));
        }
    }
}
