// 1.13c D2Game+0x160B0: monster AI callback "spirit" (table entry). ecx = game, edx = unit, stack: the
// parameter block. With the control's word +0x14 set the unit idles 50 frames; out of melee range it idles
// 10; otherwise the word is set and the unit attacks the target (mode 4, speed bit 0x10 of byte +0x104).
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g160b0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t ctl = e.rd32(tp);
    if (e.rd32(ctl + 0x14)) return gaiatk::Idle(e, game, unit, 0x32);
    if (!e.rd32(tp + 0x18)) return gaiatk::Idle(e, game, unit, 0xA);
    e.wr32(ctl + 0x14, 1);
    return gaiatk::Mode(e, game, unit, 4, e.rd32(tp + 8), 0x104, 0x10);
}
}
