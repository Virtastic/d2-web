// 1.13c D2CMP+0xA070 (316 bytes): three alpha-blend tables (75%, 50%, 25%) at block+0x3500.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0A070
// Table k (alpha 0xBF, 0x7F, 0x3F) at +0x3500 + k*0x10000: out[j + i*256] = nearest of
// (dst[c]*(255-alpha) + src[c]*alpha)/255 with src = entry j (outer), dst = entry i (inner).
extern "C" void d2cmp_a070(uint8_t* block) {
    for (uint32_t k = 0; k < 3; ++k) {
        uint32_t al = k == 0 ? 0xbf : k == 1 ? 0x7f : 0x3f, ia = 0xff - al;
        uint8_t* out = block + 0x3500 + k * 0x10000;
        for (uint32_t j = 0; j < 0x100; ++j) {
            const uint8_t* s = block + j * 4;
            for (uint32_t i = 0; i < 0x100; ++i) {
                const uint8_t* d = block + i * 4;
                out[j + i * 0x100] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)(((d[0] * ia + s[0] * al) / 0xff) & 0xff),
                                                             (int32_t)(((d[1] * ia + s[1] * al) / 0xff) & 0xff),
                                                             (int32_t)(((d[2] * ia + s[2] * al) / 0xff) & 0xff));
            }
        }
    }
}
