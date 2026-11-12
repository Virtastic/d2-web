// 1.13c D2Common+0x14F90 -- placement step (ecx = the work record), four directions times two sides: a start direction of -1 draws the direction (2 bits) and the side (1 bit, kept at +0x1B0[i]) from the record seed; otherwise the direction advances by the side at +0x174[i] modulo 4 and the side toggles, answering 0 when both come back to the start. The rectangle becomes 0x60 by 0x38 on an odd direction and 0x38 by 0x60 on an even one, then goes beside its anchor with a 16 unit offset. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f14f90(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    const uint32_t a = t + 0x174 + i * 4, b = t + 0x1b0 + i * 4;
    uint32_t u;
    if (e.rd32(s) == 0xffffffffu) {
        e.wr32(s, roll(e, t) & 3);
        e.wr32(d, e.rd32(s));
        e.wr32(b, roll(e, t) & 1);
        u = e.rd32(b);
    } else {
        const uint32_t n = (uint32_t)((int32_t)(e.rd32(d) + e.rd32(a)) % 4);
        u = (uint32_t)((int32_t)(e.rd32(a) + 1) % 2);
        if (n == e.rd32(s) && u == e.rd32(b)) return 0;
        e.wr32(d, n);
    }
    e.wr32(a, u);
    const uint32_t odd = (e.rd32(d) & 0x80000001u) != 0;
    e.wr32(o + 0x10, odd ? 0x60 : 0x38);
    e.wr32(o + 0x14, (e.rd32(d) & 0x80000001u) != 0 ? 0x38 : 0x60);
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), pw = e.rd32(p + 8), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    (void)px; (void)py; (void)pw; (void)ph; (void)sw; (void)sh;
    auto set = [&](uint32_t x, uint32_t y) { e.wr32(o + 8, x); e.wr32(o + 0xc, y); };
    if (e.rd32(a) == 1) {
    switch (e.rd32(d)) {
    case 0: set(px - 0x10, ph + py); break;
    case 1: set(px - sw, py - 0x10); break;
    case 2: set(pw - sw + px + 0x10, py - sh); break;
    case 3: set(pw + px, ph - sh + 0x10 + py); break;
    default: break;
    }
    } else {
    switch (e.rd32(d)) {
    case 0: set(pw - sw + px + 0x10, ph + py); break;
    case 1: set(px - sw, ph - sh + 0x10 + py); break;
    case 2: set(px - 0x10, py - sh); break;
    case 3: set(pw + px, py - 0x10); break;
    default: break;
    }
    }
    return 1;
}
}
