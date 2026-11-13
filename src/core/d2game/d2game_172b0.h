// 1.13c D2Game+0x172B0: monster AI callback "Griswold" (table entry). ecx = game, edx = unit, stack: the
// parameter block. In melee range a seed step modulo 100 below 80 attacks the target (mode 4, speed bit 0x10
// of byte +0x104); out of range a step below 50 walks to it; otherwise the unit idles 10 frames.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g172b0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t tgt = e.rd32(tp + 8);
    if (e.rd32(tp + 0x18)) {
        if (gaiatk::Pct(e, unit) < 0x50) return gaiatk::Mode(e, game, unit, 4, tgt, 0x104, 0x10);
    } else if (gaiatk::Pct(e, unit) < 0x32) return gaiatk::Approach(e, game, unit, tgt);
    return gaiatk::Idle(e, game, unit, 0xA);
}
}
