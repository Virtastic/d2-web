// 1.13c D2Client+0xC19B0 drawPlayerNamePlate: with a nonempty string at +0x11C780 and a type-1 unit from 0x51A80, draws it through 0xBF370 (font 6, y = D2Win #10083 + 0x27, colour 4 when
// the unit's flag byte +0x16 has bit 8, 3 when bit 4). Unicode::strlen is a pure D2Lang import (native).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_c19b0(D2cHost h, const uint16_t* name) {
    if (!name[0]) return;
    D2cCall c = {}; c.rva = 0x51A80; const uint8_t* u = (const uint8_t*)(uintptr_t)h(&c);
    if (!u) return;
    if (*(const uint32_t*)u != 1) return;
    const uint8_t* p = (const uint8_t*)(uintptr_t)*(const uint32_t*)(u + 0x14);
    uint32_t esi = 0;
    if (p && (p[0x16] & 8)) esi = 4;
    if (p && (p[0x16] & 4)) esi = 3;
    c = {}; c.rva = 0xD38A; uint32_t y = h(&c);                                           // D2Win #10083
    c = {}; c.rva = 0xBF370; c.ebx = 0x6fbcc780u; c.s[0] = 6; c.s[1] = y + 0x27; c.s[2] = esi; h(&c);
}
