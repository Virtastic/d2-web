// 1.13c D2Game+0x7D400: monster AI callback "hell meteor" (table entry). ecx = game, edx = unit, stack: the
// parameter block. Without a skill (record word +0x170 negative), or with a seed step modulo 100 not below the
// difficulty word +0x56, the unit idles the difficulty word +0x5C. Otherwise the skill lands near the unit:
// each coordinate is the unit's (types 2, 4 and 5 keep them at +0xC and +0x10 of the block +0x2C; the other
// types ask 0xA454 and 0xA3B8 with that block, 0 without one) minus the difficulty word +0x62 plus a bounded
// roll (0x11D0: ecx = the unit's seed, esi = twice that word). 0xF2380: eax = x with its low byte replaced by
// the record's byte +0x180, ecx = unit, ebx = game; stack: skill, 0, x, y.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g7d400 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t row = e.rd32(tp + 0x1C);
    if (e.rd16s(row + 0x170) < 0 || gaiatk::Pct(e, unit) >= gaiatk::Word(e, game, row, 0x56))
        return gaiatk::Idle(e, game, unit, (uint32_t)gaiatk::Word(e, game, row, 0x5C));
    auto coord = [&](uint32_t off, uint32_t ask) {
        const uint32_t t = e.rd32(unit), p = e.rd32(unit + 0x2C);
        const uint32_t c = (t == 2 || t == 4 || t == 5) ? e.rd32(p + off) : p ? e.h(ask, 0, 0, 0, 0, 0, p) : 0;
        const uint32_t r = e.h(0x11D0, 0, unit + 0x20, 0, 0, (uint32_t)(gaiatk::Word(e, game, row, 0x62) << 1));
        return r + (c - (uint32_t)gaiatk::Word(e, game, row, 0x62));
    };
    const uint32_t x = coord(0xC, 0xA454);
    const uint32_t y = coord(0x10, 0xA3B8);
    return e.h(0xF2380, (x & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), 0, x, y);
}
}
