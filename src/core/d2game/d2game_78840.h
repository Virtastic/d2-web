// 1.13c D2Game+0x78840: unit mode change callback toward a point (table entry). ecx = the game, edx = the unit,
// three stack words (ret 0xC): the mode and the two coordinates. The unit's path (+0x2C) takes the point
// (0xAB2C: path, x, y), then the shared head (pending event dropped, mode set), the unit's target cleared
// (0xACD0: unit, 0) and the shared tail. Bit 6 of the unit's flags (+0xC4) is cleared and the result is that of
// the final update (0xA0830: the game on the stack, esi = unit).
#pragma once
#include <cstdint>
#include "d2game_modeseq.h"

namespace g78840 {
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode, uint32_t x, uint32_t y)
{
    e.h(0xAB2C, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C), x, y);
    d2gmodeseq::Head(e, game, unit, mode);
    e.h(0xACD0, 0, 0, 0, 0, 0, unit, 0);
    d2gmodeseq::Tail(e, game, unit);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFBFu);
    return e.h(0xA0830, 0, 0, 0, 0, unit, game);
}
}
