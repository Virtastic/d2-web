// 1.13c D2Common+0x144C0 -- placement step (ecx = the work record): zeroes the start direction at +0x138[i] and the direction at +0xFC[i] of the current rectangle i (+0x1EC), then puts rectangle i at the anchor rectangle (index at link-table entry i +8): direction 0 is the anchor x and the anchor bottom edge. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f144c0(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    e.wr32(s, 0);
    e.wr32(d, e.rd32(s));
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), pw = e.rd32(p + 8), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    (void)px; (void)py; (void)pw; (void)ph; (void)sw; (void)sh;
    auto set = [&](uint32_t x, uint32_t y) { e.wr32(o + 8, x); e.wr32(o + 0xc, y); };
    switch (e.rd32(d)) {
    case 0: set(px, py + ph); break;
    case 1: set(px - sw, py); break;
    case 2: set(pw - sw + px, py - sh); break;
    case 3: set(px + pw, ph - sh + py); break;
    default: break;
    }
    return 1;
}
}
