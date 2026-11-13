// 1.13c D2Game+0x16410: monster AI callback "bone wall" (table entry). ecx = game, edx = unit, stack: the
// parameter block. Until the game frame (+0xA8) passes the control's word +0x14 the unit idles 15 frames;
// after it the unit dies (mode 0).
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g16410 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t ctl = e.rd32(tp);
    if ((int32_t)e.rd32(ctl + 0x14) >= (int32_t)e.rd32(game + 0xA8)) return gaiatk::Idle(e, game, unit, 0xF);
    return gaiatk::Mode(e, game, unit, 0, 0);
}
}
