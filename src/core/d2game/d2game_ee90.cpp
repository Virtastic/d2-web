// src/core/d2game/d2game_ee90.cpp -- 1.13c D2Game+0x0EE90.
// Stdcall, ret 8: a0 = unit (its +4 type/class, +0x80 data, data+0x6D table index), a1 = the unit whose +0x0C is the level, +0x30 a cached tier, +0x80 flag byte.
// Walks the tiers 7..2 (DAT_6fd1d09c); each rolls 0x11D0 (ECX = seed of 0xA5EC(a0), ESI = the tier's remaining count); a roll of zero (or, on the player path, a count <= 0) names the tier.
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
static inline uint32_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
// 1.13c D2Game+0x0EE90
extern "C" uint32_t D2Game_sub_EE90(const D2GameSeam& S, uint32_t a0, uint32_t a1)
{
    auto fatal = [&](uint32_t line) -> uint32_t {
        const uint32_t m = S(0xA2DA, 0, 0, 0, 0, 0, line);
        S(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m);
        S(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return 0;
    };
    auto pl = [&]() { return (uint32_t)(uint16_t)S(0xB37E, 0, 0, 0, 0, 0, a0); };
    if (pl() >= 1) { const uint32_t c = rd32(a1, 0x30); if (c) return c; }
    if (!a0) return fatal(0xa2a);
    const uint32_t pd = rd32(a0, 0x80);
    if (!pd) return fatal(0x482);
    const uint32_t bias = pl() >= 1 ? 100 : 0;
    const uint32_t rec = S(0xB42C, 0, 0, 0, 0, 0, rd32(a0, 4), rd8(pd, 0x6d), bias);
    if (!rec) return fatal(0x486);
    const uint32_t info = S(0xA53E, 0, 0, 0, 0, 0, rd32(a0, 4));
    if (!info) return fatal(0x489);
    if (rd8(info, 0x12a)) return 2;
    int32_t lvl = (int32_t)rd32(a1, 0xc);
    int32_t esi = 0;
    if (pl() >= 1 && (S(0xA430, 0, 0, 0, 0, 0, a0, 0x34) != 0 || (lvl -= (int32_t)rd8(info, 0xfd)) < 1)) lvl = 1;
    uint32_t tier = 1;
    const bool player = pl() >= 1;
    static const uint32_t order[6] = { 7, 6, 5, 4, 3, 2 };
    auto R = [&](uint32_t i) { return (int32_t)rd32(rec, i * 4); };
    for (int i = 0; i < 6; ++i) {
        int32_t div = 0; bool have = true;
        switch (order[i]) {
        case 7: esi = R(0);  div = player ? lvl / R(1)  : lvl;               tier = 7; break;
        case 6: esi = R(3);  div = player ? lvl / R(4)  : lvl;               tier = 6; break;
        case 5: esi = R(6);  div = player ? lvl / R(7)  : lvl;               tier = 5; break;
        case 4: esi = R(9);  div = lvl / R(10); tier = 4; break;
        case 3: esi = R(12); div = lvl / R(13); tier = 3; break;
        case 2: esi = R(14); div = lvl / R(15); tier = 2; break;
        default: have = false; break;
        }
        if (have) esi -= div;
        if (player) { if (esi <= 0) return tier; }
        else if (esi <= 0) esi = 1;
        if (S(0x11D0, 0, S(0xA5EC, 0, 0, 0, 0, 0, a0), 0, 0, (uint32_t)esi) == 0) return tier;
    }
    return ((rd8(a1, 0x80) & 0x40) | 0x20) >> 5;
}
