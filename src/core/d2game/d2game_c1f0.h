// 1.13c D2Game+0xC1F0 (FindUseableCorpse): esi = pUnit, stack = (pGame, radius), ret 8; callers 0xAA710, 0xAB070.
// Written from the 1.13c disassembly as a rule over an environment E (see skill_helpers_a19a0.h). Rule: the unit's room
// (0xA478); its x / y are the fixed-position words path+0xC / +0x10 for unit types 2 and 4..5, else 0xA454 / 0xA3B8 of
// the path (0 when there is none); a zeroed 14-dword find block (flags 0x1002, unit, x, y, radius) goes with the room
// to 0xA502, which fills a list through 0xA4F0 (count at block word 6, candidate array pointer at word 1); the first
// candidate that 0xB450 accepts and 0xE0A60(candidate, unit, game) accepts is returned, else 0; 0xA4FC frees the list on both exits.
#pragma once
#include <cstdint>

namespace c1f0 {
template <class E> uint32_t FindUseableCorpse(E& e, uint32_t unit, uint32_t game, uint32_t radius)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    const int32_t type = (int32_t)e.rd32(unit);
    const bool fixedPos = type == 2 || (type > 3 && type < 6);
    const uint32_t path = e.rd32(unit + 0x2C);
    uint32_t x, y;
    if (fixedPos) x = e.rd32(path + 0xC);
    else x = path ? e.h(0xA454, 0, 0, 0, 0, 0, path) : 0;
    if (fixedPos) y = e.rd32(path + 0x10);
    else y = path ? e.h(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    uint32_t find[14] = {};
    find[0] = 0x1002; find[2] = unit; find[3] = x; find[4] = y; find[5] = radius;
    uint32_t list[8] = {};
    e.h(0xA502, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), e.stk(list), room, x);
    e.blk(0xA502, find, 14);
    e.h(0xA4F0, 0, 0, 0, 0, 0, e.stk(list));
    for (int32_t i = 0; i < (int32_t)list[6]; i++) {
        const uint32_t cand = e.rd32(list[1] + (uint32_t)i * 4);
        if (e.h(0xB450, 0, 0, 0, 0, 0, cand) && e.h(0xE0A60, cand, unit, 0, 0, 0, game)) {
            e.h(0xA4FC, 0, 0, 0, 0, 0, e.stk(list));
            return cand;
        }
    }
    e.h(0xA4FC, 0, 0, 0, 0, 0, e.stk(list));
    return 0;
}
}
