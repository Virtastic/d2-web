// 1.13c D2Common+0x40F00 -- placement step (ecx = the work record): rolls the orientation (0 or 1) of the current rectangle, stores it as both the current and the start value, sizes the rectangle 64 by 160 or 160 by 64 and puts it at the position the level record of the id at +0x1F0 holds at +0x24 and +0x28 (records of 0x9C bytes at the pointer at image offset 0xA0B38, count at +0xC5C of the data tables). Answers 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_99e1c;
__attribute__((weak)) uint32_t g_d2common_a0b38;
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f40f00(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    const uint32_t u = roll(e, t) & 1;
    e.wr32(d, u);
    e.wr32(s, u);
    if (u == 0) { e.wr32(o + 0x10, 0x40); e.wr32(o + 0x14, 0xa0); }
    else { e.wr32(o + 0x10, 0xa0); e.wr32(o + 0x14, 0x40); }
    const int32_t id = (int32_t)e.rd32(t + 0x1f0);
    const uint32_t rec = (id < 0 || id >= (int32_t)e.rd32(g_d2common_99e1c + 0xc5c)) ? 0 : (uint32_t)id * 0x9c + g_d2common_a0b38;
    e.wr32(o + 8, e.rd32(rec + 0x24));
    e.wr32(o + 0xc, e.rd32(rec + 0x28));
    return 1;
}
}
