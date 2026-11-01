// 1.13c D2Game+0x4F450: esi = unit, ebx = caller word (void). A type-0 unit with subtype 3 builds a 28-word request (flags 0x1000, word1 = 4, words 2 and 19 = #A3A0(unit) / 8) and hands it to 0xDE0C0 and 0xDCF20.
#include "d2game_seam4e.h"
extern "C" void d2game_4f450(uint32_t unit, uint32_t ebx, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 0 || d2rd32(unit + 4) != 3) return;
    const int32_t x = (int32_t)e->call(0xA3A0, 0, 0, 0, 0, 0, unit);
    uint32_t L[28] = {};
    L[1] |= 4;
    const int32_t v = (x < 0 ? x + 7 : x) >> 3;
    L[2] = (uint32_t)v; L[19] = (uint32_t)v;
    L[0] = 0x1000;
    e->call(0xDE0C0, unit, 0, 0, 0, 0, ebx, unit, 0, (uint32_t)(uintptr_t)L);
    e->blk(0xDE0C0, L, 28);
    e->call(0xDCF20, unit, 0, 0, 0, 0, ebx, unit, (uint32_t)(uintptr_t)L);
    e->blk(0xDCF20, L, 28);
}
