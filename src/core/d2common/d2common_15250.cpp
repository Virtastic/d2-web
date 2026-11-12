// 1.13c D2Common+0x15250 -- placement step (ecx = the work record), four directions times two sides, as the 16-unit variant but the rectangle keeps its size and the offset is 8 units along the anchor edge. Returns 1, or 0 when direction and side come back to the start.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f15250(GE& e, uint32_t t)
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
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), pw = e.rd32(p + 8), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    (void)px; (void)py; (void)pw; (void)ph; (void)sw; (void)sh;
    auto set = [&](uint32_t x, uint32_t y) { e.wr32(o + 8, x); e.wr32(o + 0xc, y); };
    if (e.rd32(a) == 1) {
    switch (e.rd32(d)) {
    case 0: set(px, ph + py); break;
    case 1: set(px - sw, py + 8); break;
    case 2: set(pw - sw + px, py - sh); break;
    case 3: set(pw + px, ph - sh - 8 + py); break;
    default: break;
    }
    } else {
    switch (e.rd32(d)) {
    case 0: set(pw - sw + px, ph + py); break;
    case 1: set(px - sw, ph - sh - 8 + py); break;
    case 2: set(px, py - sh); break;
    case 3: set(pw + px, py + 8); break;
    default: break;
    }
    }
    return 1;
}
}
