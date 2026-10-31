// 1.13c D2CMP+0xA640 (196 bytes): 256x256 additive (saturating) blend table at block+0x33500.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0A640
// out[j + i*256] = nearest of min(a[c]+b[c], 255), a = entry j (outer), b = entry i (inner).
extern "C" void d2cmp_a640(uint8_t* block) {
    uint8_t* out = block + 0x33500;
    for (uint32_t j = 0; j < 0x100; ++j) {
        const uint8_t* a = block + j * 4;
        for (uint32_t i = 0; i < 0x100; ++i) {
            const uint8_t* b = block + i * 4;
            uint32_t c0 = (uint32_t)b[0] + a[0], c1 = (uint32_t)b[1] + a[1], c2 = (uint32_t)a[2] + b[2];
            if (c0 > 0xfe) c0 = 0xff;
            if (c1 > 0xfe) c1 = 0xff;
            if (c2 > 0xfe) c2 = 0xff;
            out[j + i * 0x100] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)c0, (int32_t)c1, (int32_t)c2);
        }
    }
}
