// 1.13c D2Client+0xC1A20 drawTwoLineLabel: for a type-1 unit from 0x51A80 draws the string at +0x11CA30 (font 6, y = #10083 + 0x27, colour = dword +0x11CC30) through 0xBF370, then the string at
// +0x11C578 (colour = dword +0x11C778) one line lower when either of the first two strings is nonempty. s1 = +0x11CA30, s0 = +0x11C780 (first words only).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_c1a20(D2cHost h, uint16_t s1first, uint16_t s0first, uint32_t colA, uint32_t colB) {
    D2cCall c = {}; c.rva = 0x51A80; const uint8_t* u = (const uint8_t*)(uintptr_t)h(&c);
    if (!u || *(const uint32_t*)u != 1) return;
    c = {}; c.rva = 0xD38A; uint32_t y = h(&c) + 0x27;
    c = {}; c.rva = 0xBF370; c.ebx = 0x6fbcca30u; c.s[0] = 6; c.s[1] = y; c.s[2] = colA; h(&c);
    if (s1first != 0 || s0first != 0) { c = {}; c.rva = 0xD38A; y += h(&c); }
    c = {}; c.rva = 0xBF370; c.ebx = 0x6fbcc578u; c.s[0] = 6; c.s[1] = y; c.s[2] = colB; h(&c);
}
