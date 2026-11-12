// 1.13c D2Game+0x787C0: unit mode change callback toward a target unit (table entry). ecx = the game, edx = the
// unit, two stack words (ret 8): the mode and the target. After the shared head a null target is the fatal assert
// at line 0x4BE (whose exit result then stands in for the target). The target is set (0xACD0: unit, target), then
// the shared tail; with a unit, bit 6 of its flags (+0xC4) is cleared and the result is that of the final update
// (0xA0830: the game on the stack, esi = unit), otherwise the tail's own result.
#pragma once
#include <cstdint>
#include "d2game_modeseq.h"

namespace g787c0 {
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode, uint32_t target)
{
    d2gmodeseq::Head(e, game, unit, mode);
    if (!target) {
        const uint32_t s = e.h(0xA2DA, 0, 0, 0, 0, 0, 0x4BE);
        e.h(0xA2C2, 0, 0, 0, 0, 0, 0xF9FF9u, s);
        target = e.h(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    }
    e.h(0xACD0, 0, 0, 0, 0, 0, unit, target);
    const uint32_t r = d2gmodeseq::Tail(e, game, unit);
    if (!unit) return r;
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFBFu);
    return e.h(0xA0830, 0, 0, 0, 0, unit, game);
}
}
