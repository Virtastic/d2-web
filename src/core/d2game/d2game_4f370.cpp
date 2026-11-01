// 1.13c D2Game+0x4F370: ecx = request holder (->+0xC points at a descriptor {block*, p1, p2, limit}), edx = unit. Copies a 28-word block, picks two unit properties (direct fields for unit types 2,4,5, else #A3B8/#A454 of unit->+0x2C), clears block word 2 when limit < #AE26(p1, p2, ...), then hands the block to 0xDE0C0 and 0xDCF20.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4f370(uint32_t self, uint32_t unit, const D2Seam* e)
{
    const uint32_t d = d2rd32(self + 0xC);
    if (!d) return 0;
    uint32_t L[28];
    memcpy(L, (const void*)(uintptr_t)d2rd32(d), sizeof L);
    const int32_t type = (int32_t)d2rd32(unit);
    const uint32_t sub = d2rd32(unit + 0x2C);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    const uint32_t a = direct ? d2rd32(sub + 0x10) : (sub ? e->call(0xA3B8, 0, 0, 0, 0, 0, sub) : 0);
    const uint32_t b = direct ? d2rd32(sub + 0xC) : (sub ? e->call(0xA454, 0, 0, 0, 0, 0, sub) : 0);
    const int32_t r = (int32_t)e->call(0xAE26, 0, 0, 0, 0, 0, d2rd32(d + 4), d2rd32(d + 8), b, a);
    if ((int32_t)d2rd32(d + 0xC) < r) L[2] = 0;
    const uint32_t p0 = d2rd32(self), p1 = d2rd32(self + 4);
    e->call(0xDE0C0, unit, 0, 0, 0, 0, p0, p1, 1, (uint32_t)(uintptr_t)L);
    e->blk(0xDE0C0, L, 28);
    e->call(0xDCF20, unit, 0, 0, 0, 0, p0, p1, (uint32_t)(uintptr_t)L);
    e->blk(0xDCF20, L, 28);
    return 1;
}
