// 1.13c D2CMP+0x9E60 (106 bytes): 32 darkening tables at block+0x400.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09E60
// Level k = 1..32, entry i: channel c' = c*k >> 5, nearest of the block's 256 entries.
extern "C" void d2cmp_9e60(uint8_t* block) {
    uint8_t* out = block + 0x400;
    for (uint32_t k = 1; k <= 0x20; ++k, out += 0x100)
        for (uint32_t i = 0; i < 0x100; ++i) {
            const uint8_t* p = block + i * 4;
            out[i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)(p[0] * k >> 5), (int32_t)(p[1] * k >> 5), (int32_t)(p[2] * k >> 5));
        }
}
