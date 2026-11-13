// 1.13c D2Game+0x189E0: monster AI callback "siege tower" (table entry). ecx = game, edx = unit, stack: the
// parameter block. A unit whose flag word +0xC8 has bit 10 and whose owner (0xE03A0: eax = unit +0x94, ecx =
// game, edx = unit +0x98) is there and alive (0xDFA90: eax = owner, answer 0) only idles. Otherwise, with a
// target, a unit is looked for (0xB17B0: eax 1, ecx = the pair {400, 0}, edx = game; stack: unit, the filter
// 0x15B00) and a hit is handed to 0x158F0 (esi = the hit; stack: unit) before an idle. Every path ends with an
// idle of the record's difficulty word +0x56 frames.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g189e0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t mod = 0xB2000000u)
{
    const uint32_t row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    bool held = false;
    if (unit && ((e.rd32(unit + 0xC8) >> 10) & 1)) {
        const uint32_t owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
        held = owner && !e.h(0xDFA90, owner);
    }
    if (!held && tgt) {
        uint32_t pair[2] = { 0x190, 0 };
        const uint32_t hit = e.h(0xB17B0, 1, e.stk(pair), game, 0, 0, unit, mod | 0x15B00);
        e.blk(0xB17B0, pair, 2);
        if (hit) {
            e.h(0x158F0, 0, 0, 0, 0, hit, unit);
            gaiatk::Idle(e, game, unit, (uint32_t)gaiatk::Word(e, game, row, 0x56));
        }
    }
    return gaiatk::Idle(e, game, unit, (uint32_t)gaiatk::Word(e, game, row, 0x56));
}
}
