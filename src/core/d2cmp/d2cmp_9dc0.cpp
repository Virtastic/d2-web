// 1.13c D2CMP+0x9DC0 (147 bytes): 16 tint-toward-white tables at block+0x2400.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09DC0
// Level k = 1..16, entry i: channel c' = c + ((255-c)*k >> 4), nearest of the block's 256 entries.
extern "C" void d2cmp_9dc0(uint8_t* block) {
    uint8_t* out = block + 0x2400;
    for (uint32_t k = 1; k <= 0x10; ++k, out += 0x100)
        for (uint32_t i = 0; i < 0x100; ++i) {
            const uint8_t* p = block + i * 4;
            out[i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)(((0xffu - p[0]) * k >> 4) + p[0]),
                                             (int32_t)(((0xffu - p[1]) * k >> 4) + p[1]),
                                             (int32_t)(((0xffu - p[2]) * k >> 4) + p[2]));
        }
}
