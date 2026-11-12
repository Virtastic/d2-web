// 1.13c D2Common+0x40E10 -- placement step (ecx = the work record): the first call rolls the orientation (0 or 1) of the current rectangle and remembers it as the start; a later call flips it and answers 0 when that is the start again. The rectangle becomes 64 by 160 (orientation 0) or 160 by 64 and is put beside its anchor at the offset the table at image offset 0x9380C holds for this orientation and the orientation of the anchor. Answers 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f40e10(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    if (e.rd32(s) == 0xffffffffu) {
        e.wr32(s, roll(e, t) & 1);
        e.wr32(d, e.rd32(s));
    } else {
        const uint32_t n = e.rd32(d) == 0 ? 1 : 0;
        if (n == e.rd32(s)) return 0;
        e.wr32(d, n);
    }
    const uint32_t dir = e.rd32(d), j = e.rd32(e.rd32(t + 0xf8) + 8 + i * 16), dj = e.rd32(t + 0xfc + j * 4);
    if (dir == 0) { e.wr32(o + 0x10, 0x40); e.wr32(o + 0x14, 0xa0); }
    else { e.wr32(o + 0x10, 0xa0); e.wr32(o + 0x14, 0x40); }
    static const int32_t T[8] = { 0, -160, -96, -64, -64, -96, -160, 0 };
    const uint32_t k = dir + dj * 2;
    e.wr32(o + 8, (uint32_t)T[k * 2] + e.rd32(t + 8 + j * 16));
    e.wr32(o + 0xc, (uint32_t)T[k * 2 + 1] + e.rd32(t + 12 + j * 16));
    return 1;
}
}
