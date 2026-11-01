// 1.13c D2Game+0xCB30 (D2GAME_SummonPet_6FD14430: a ring of pets spawned around the unit's target): ecx = pGame, edx = pUnit,
// two unused stack words, ret 8. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a null unit returns 0. The unit's seed (+0x20 low, +0x24 high; low := low * 0x6AC690C5 + high, high := the carry) rolls
// count = game byte +0x6D + roll % 3 + 2. 0xAC64(unit, &a, &b, &c, &d, game byte +0x6D, 0) fills four out-dwords; 0xE07A0
// (eax = unit, ecx = game); the target is 0xA598(unit+0x2C), none when it is the unit. The centre comes from the target if any,
// else the unit: types 2, 4 and 5 read the static path (+0x2C) at +0xC / +0x10, the rest ask 0xA454 / 0xA3B8 of the path, a
// null path giving 0. Each of count rounds rolls x = cx + roll % 18 - 9, y = cy + roll % 18 - 9 (the first roll's low word is
// stored before the second is taken) and asks 0x1280 (eax = 0xA478(unit), ebx = x, edi = y; plain ret) for a room, twice; a room
// makes a spawn request 0xEEF10 (ecx = a stack block: game, room, 0, a, d, 0, x, y, -1, word 0); the spawned pet gets
// +0xC4 |= 0x4020000, its +0xDC nodes whose first byte is 2 go to 0x8E6F0 (eax = node, edi = game), then 0x8EA80(ecx = game,
// edx = pet, stack = 2, game+0xA8 + 0xF, 0, 0, 0; ret 0x14) and 0xAC76(pet, unit). Result 1.
#pragma once
#include <cstdint>

namespace cb30 {
template <class E> uint32_t Roll(E& e, uint32_t unit, uint32_t mod)
{
    const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)v); e.wr32(unit + 0x24, (uint32_t)(v >> 32));
    return (uint32_t)v % mod;
}
template <class E> uint32_t Centre(E& e, uint32_t u, bool y)
{
    const uint32_t t = e.rd32(u);
    if (t == 2 || t == 4 || t == 5) return e.rd32(e.rd32(u + 0x2C) + (y ? 0x10 : 0xC));
    const uint32_t p = e.rd32(u + 0x2C);
    return p ? e.h(y ? 0xA3B8 : 0xA454, 0, 0, 0, 0, 0, p) : 0;
}
template <class E> uint32_t SummonPet(E& e, uint32_t game, uint32_t unit)
{
    if (!unit) return 0;
    const uint32_t lvl = e.rd8(game + 0x6D);
    int32_t count = (int32_t)(lvl + Roll(e, unit, 3) + 2);
    uint32_t a = 0, b = 0, c = 0, d = 0;
    e.h(0xAC64, 0, 0, 0, 0, 0, unit, e.stk(&a), e.stk(&b), e.stk(&c), e.stk(&d));
    e.h(0xE07A0, unit, game);
    uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit) tgt = 0;
    const uint32_t src = tgt ? tgt : unit;
    const uint32_t cx = Centre(e, src, false), cy = Centre(e, src, true);
    for (; count > 0; count--) {
        const int32_t x = (int32_t)(Roll(e, unit, 0x12) + cx - 9);
        const int32_t y = (int32_t)(Roll(e, unit, 0x12) + cy - 9);
        uint32_t rm = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        const uint32_t room = e.h(0x1280, rm, 0, 0, (uint32_t)x);
        if (!room) continue;
        rm = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        e.h(0x1280, rm, 0, 0, (uint32_t)x);
        uint32_t blk[10] = { game, room, 0, a, d, 0, (uint32_t)x, (uint32_t)y, 0xFFFFFFFFu, 0 };
        const uint32_t pet = e.h(0xEEF10, 0, e.stk(blk));
        if (!pet) continue;
        e.wr32(pet + 0xC4, e.rd32(pet + 0xC4) | 0x4020000u);
        for (uint32_t nd = e.rd32(pet + 0xDC); nd;) {
            const uint32_t nx = e.rd32(nd + 0x24);
            if (e.rd8(nd) == 2) e.h(0x8E6F0, nd);
            nd = nx;
        }
        e.h(0x8EA80, 0, game, pet, 0, 0, 2, e.rd32(game + 0xA8) + 0xF, 0, 0, 0);
        e.h(0xAC76, 0, 0, 0, 0, 0, pet, unit);
    }
    return 1;
}
}
