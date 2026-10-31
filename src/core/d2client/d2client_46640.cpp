// D2Client 0x46640: releases the trade/vendor panel resources (callers 0xC2D10, 0x46A20).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
// g = the 0x44 bytes at 0x11973B (+0 [0x11973B], +4 [0x11973F], +0x1C..+0x3B the eight slots 0x119757..0x119776, +0x40 [0x11977B]);
// h = the two dwords at 0x1197C0/0x1197C4. Imports go through host under their thunk RVAs: 0xCE32 (Ordinal_10901, 2 stack words),
// 0xC18A (Ordinal_10065, 1 stack word), 0xBF86 (Ordinal_10043: ECX = handle, EDX = 0x6FB86AB0 the source-file string, stack 0x7A, 0);
// 0xA3D20 takes nothing; [0x11977B]'s first vtable slot is called with 1 (host rva 0x11977B, ecx = the object).
static uint32_t rd(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static void wr(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }
// 1.13c D2Client+0x46640
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46640(uint8_t* g, uint32_t* h, D2cHost host) {
    D2cCall c = {}; c.rva = 0xCE32; c.s[1] = rd(g); host(&c);
    c = {}; c.rva = 0xCE32; c.s[1] = rd(g + 4); host(&c);
    wr(g, 0); wr(g + 4, 0);
    for (int i = 0; i < 2; ++i) if (h[i]) {
        c = {}; c.rva = 0xC18A; c.s[0] = h[i]; host(&c);
        c = {}; c.rva = 0xBF86; c.ecx = h[i]; c.edx = 0x6FB86AB0u; c.s[0] = 0x7A; host(&c);
        h[i] = 0;
    }
    if (const uint32_t o = rd(g + 0x40)) { c = {}; c.rva = 0x11977B; c.ecx = o; c.s[0] = 1; host(&c); wr(g + 0x40, 0); }
    static const uint8_t order[8] = { 0x1C, 0x20, 0x24, 0x28, 0x34, 0x38, 0x2C, 0x30 };
    for (uint8_t off : order) if (rd(g + off)) { c = {}; c.rva = 0xA3D20; host(&c); wr(g + off, 0); }
}
