// 1.13c D2Common+0x14D00 -- placement step (ecx = the work record), eight directions: a start direction of -1 draws one from the record seed (low 3 bits) and uses it; otherwise the direction advances by one modulo 8 and the step answers 0 when it comes back to the start. Directions 0-1 put the rectangle under its anchor, 2-3 left of it, 4-5 above it, 6-7 right of it. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f14d00(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    if (e.rd32(s) == 0xffffffffu) {
        e.wr32(s, roll(e, t) & 7);
        e.wr32(d, e.rd32(s));
    } else {
        const uint32_t n = (uint32_t)((int32_t)(e.rd32(d) + 1) % 8);
        if (n == e.rd32(s)) return 0;
        e.wr32(d, n);
    }
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), pw = e.rd32(p + 8), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    (void)px; (void)py; (void)pw; (void)ph; (void)sw; (void)sh;
    auto set = [&](uint32_t x, uint32_t y) { e.wr32(o + 8, x); e.wr32(o + 0xc, y); };
    switch (e.rd32(d)) {
    case 0: case 1: set(px, ph + py); break;
    case 2: case 3: set(px - sw, py); break;
    case 4: case 5: set(px, py - sh); break;
    case 6: case 7: set(pw + px, py); break;
    default: break;
    }
    return 1;
}
}
