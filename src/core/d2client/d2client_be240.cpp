// 1.13c D2Client+0xBE240 (ESI = mode): D2Common #10014(esi); 0 gives D2Lang #10003(0x150D); mode != 8 gives r + 0x16E; mode 8: fatal path when +0x11C0A0 is 0, then #10003(0x13B8 / 0x13B7 by byte +0x115591).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_be240(D2cHost h, uint32_t esi, uint32_t cc0a0, uint32_t c5591) {
    D2cCall c = {}; c.rva = 0xC214; c.s[0] = esi; uint32_t r = h(&c);
    if (r == 0) { c = {}; c.rva = 0xD1EC; c.ecx = 0x150d; return h(&c); }
    if (esi != 8) return r + 0x16e;
    if (cc0a0 == 0) {
        c = {}; c.rva = 0xBF92; c.s[0] = 0x60; uint32_t t = h(&c);                       // Fog #10265 (cdecl)
        c = {}; c.rva = 0xBF80; c.s[0] = 0x6fb8359f; c.s[1] = t; h(&c);                  // Fog #10024 (cdecl)
        c = {}; c.rva = 0x3664; c.s[0] = 0xffffffffu; h(&c);                              // exit(-1)
    }
    c = {}; c.rva = 0xD1EC; c.ecx = (c5591 & 0xff) ? 0x13b8 : 0x13b7; return h(&c);
}
