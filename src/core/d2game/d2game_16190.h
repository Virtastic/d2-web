// 1.13c D2Game+0x16190: monster AI callback "catapult" (table entry). ecx = game, edx = unit, stack: the
// parameter block. One seed step modulo 100 below the record's difficulty word +0x56: the unit attacks
// without a target (mode 4, speed bit 0x10 of byte +0x104); otherwise it idles 15 frames.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g16190 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t row = e.rd32(tp + 0x1C);
    if (gaiatk::Pct(e, unit) >= gaiatk::Word(e, game, row, 0x56)) return gaiatk::Idle(e, game, unit, 0xF);
    return gaiatk::Mode(e, game, unit, 4, 0, 0x104, 0x10);
}
}
