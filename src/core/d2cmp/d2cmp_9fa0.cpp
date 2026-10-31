// 1.13c D2CMP+0x9FA0 (201 bytes): 256x256 multiply-blend table at block+0x43500.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09FA0
// out[j + i*256] = nearest of (a[c]*b[c]/255) for a = entry i, b = entry j; j outer, i inner.
extern "C" void d2cmp_9fa0(uint8_t* block) {
    uint8_t* out = block + 0x43500;
    for (uint32_t j = 0; j < 0x100; ++j) {
        const uint8_t* b = block + j * 4;
        for (uint32_t i = 0; i < 0x100; ++i) {
            const uint8_t* a = block + i * 4;
            out[j + i * 0x100] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)((a[0] * b[0] / 0xffu) & 0xff),
                                                         (int32_t)((b[1] * a[1] / 0xffu) & 0xff), (int32_t)((a[2] * b[2] / 0xffu) & 0xff));
        }
    }
}
