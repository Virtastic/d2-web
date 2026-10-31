// 1.13c D2CMP+0xA250 (export #10039, 141 bytes): map n packed 3-byte colours to palette indices.
#include "d2cmp_palstate.h"
uint32_t g_d2cmp_dark_6fe88320;  // 0x6FE88320
// 1.13c D2CMP+0x0A250
// Only when a palette is loaded (0x6FE88318). cb defaults to 0x9A10. Each 3-byte source entry gives
// (b0,b1,b2); with 0x6FE88320 set and all three below 11 the triple becomes (4,0,0). Retail pushes
// the three as dwords whose upper bytes are stale argument slots; the callback reads bytes only.
// Always returns 0.
extern "C" uint32_t d2cmp_a250(const uint8_t* src, uint8_t* dst, int32_t n, uint32_t (*cb)(uint8_t, uint8_t, uint8_t)) {
    if (g_d2cmp_loaded_6fe88318 == 0) return 0;
    if (cb == nullptr) cb = d2cmp_9a10;
    for (; n != 0; --n, src += 3) {
        uint8_t a = src[0], b = src[1], c = src[2];
        if (g_d2cmp_dark_6fe88320 != 0 && a < 0xb && b < 0xb && c < 0xb) { a = 4; b = 0; c = 0; }
        *dst++ = (uint8_t)cb(a, b, c);
    }
    return 0;
}
