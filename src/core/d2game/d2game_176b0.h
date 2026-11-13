// 1.13c D2Game+0x176B0: monster AI callback "the Smith" (table entry). ecx = game, edx = unit, stack: the
// parameter block. In melee range the unit attacks the target (mode 4, speed bit 0x10 of byte +0x104). Out of
// range its life percentage (0xA718: unit), held to 0..100, gives the speed bonus (100 - percentage) / 2; a
// bonus other than 0 is written to the word +0x1C of the monster's path (monster data +0x2C), and the unit
// walks to the target.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g176b0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t tgt = e.rd32(tp + 8);
    if (e.rd32(tp + 0x18)) return gaiatk::Mode(e, game, unit, 4, tgt, 0x104, 0x10);
    int32_t v = (int32_t)e.h(0xA718, 0, 0, 0, 0, 0, unit);
    if (v < 0) v = 0; else if (v > 100) v = 100;
    const int32_t c = (100 - v) >> 1;
    const uint32_t md = unit && e.rd32(unit) == 1 ? e.rd32(unit + 0x14) : 0;
    const uint32_t path = md ? e.rd32(md + 0x2C) : 0;
    if (c) e.wr32(path + 0x1C, (uint32_t)c);
    return gaiatk::Approach(e, game, unit, tgt);
}
}
