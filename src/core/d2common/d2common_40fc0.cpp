// 1.13c D2Common+0x40FC0 -- placement step (ecx = the work record): rolls the orientation (0 or 1) of the current rectangle, stores it as both the current and the start value, sizes the rectangle 64 by 160 or 160 by 64 and puts it left of its anchor: x = anchor x - width, y = anchor y + anchor height - height - 16. Answers 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f40fc0(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    const uint32_t u = roll(e, t) & 1;
    e.wr32(d, u);
    e.wr32(s, u);
    if (u == 0) { e.wr32(o + 0x10, 0x40); e.wr32(o + 0x14, 0xa0); }
    else { e.wr32(o + 0x10, 0xa0); e.wr32(o + 0x14, 0x40); }
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    e.wr32(o + 8, px - sw);
    e.wr32(o + 0xc, ph - sh - 0x10 + py);
    return 1;
}
}
