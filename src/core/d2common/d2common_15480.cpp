// 1.13c D2Common+0x15480 -- placement step (ecx = the work record), eight directions with half-size offsets: the step rule of the eight-direction variant; the rectangle goes under (0, 1), left of (2, 3), above (4, 5) or right of (6, 7) its anchor, shifted by half its own width or height plus 8 to one side or the other. Returns 1, or 0 when the direction comes back to the start.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f15480(GE& e, uint32_t t)
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
    case 0: set(px - 8 - (uint32_t)((int32_t)sw / 2), ph + py); break;
    case 1: set(px + (uint32_t)((int32_t)sw / 2) + 8, ph + py); break;
    case 2: set(px - sw, py - (uint32_t)((int32_t)sh / 2) - 8); break;
    case 3: set(px - sw, (uint32_t)((int32_t)sh / 2) + 8 + py); break;
    case 4: set(px - 8 - (uint32_t)((int32_t)sw / 2), py - sh); break;
    case 5: set(px + (uint32_t)((int32_t)sw / 2) + 8, py - sh); break;
    case 6: set(pw + px, py - (uint32_t)((int32_t)sh / 2) - 8); break;
    case 7: set(pw + px, (uint32_t)((int32_t)sh / 2) + 8 + py); break;
    default: break;
    }
    return 1;
}
}
