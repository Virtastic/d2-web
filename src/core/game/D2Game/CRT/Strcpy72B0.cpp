#include <cstdint>
#include <emscripten.h>

// 1.13c D2Game+0x072B0 (strcpy): byte-copy until the source is word aligned, then word-copy while the
// 0x7EFEFEFF trick says no zero byte is inside; once one is, store the 1/2/3/4 bytes it covers and stop.
// 1.13c D2Game+0x072B0
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2game_strcpy_72b0(uint8_t* dst, const uint8_t* src) {
    uint8_t* d = dst;
    while (((uintptr_t)src & 3) != 0) {
        uint8_t b = *src++;
        if (b == 0) { *d = b; return dst; }
        *d++ = b;
    }
    for (;;) {
        uint32_t v = *(const uint32_t*)src;
        src += 4;
        if (((v ^ 0xFFFFFFFFu ^ (v + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
            if ((uint8_t)v == 0) { *d = (uint8_t)v; return dst; }
            if ((uint8_t)(v >> 8) == 0) { *(uint16_t*)d = (uint16_t)v; return dst; }
            if ((v & 0x00FF0000u) == 0) { *(uint16_t*)d = (uint16_t)v; d[2] = 0; return dst; }
            if ((v & 0xFF000000u) == 0) { *(uint32_t*)d = v; return dst; }
        }
        *(uint32_t*)d = v;
        d += 4;
    }
}
