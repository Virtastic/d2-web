// 1.13c D2Game+0x7D550: monster AI callback, the bat demon's special state (table entry). ecx = game, edx =
// unit, stack: the parameter block. A unit in mode 0xB: the control's word +0x14 becomes 1, every type-2 entry
// of the unit's timer list (+0xDC, next at +0x24) is freed (0x8E6F0: eax = entry) and a timer of kind 2 is set
// for the next frame (0x8EA80: ecx = game, edx = unit; stack: 2, frame + 1, 0, 0, 0). Else a control word of 4
// becomes 1 and the unit goes to mode 0xA (speed bit 4 of byte +0x105). Else the control's own callback runs
// (0x9D230: ecx = game, edx = unit; stack: control, its first word) and the unit idles 1 frame.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g7d550 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t ctl = e.rd32(tp);
    if (unit && e.rd32(unit + 0x10) == 0xB) {
        e.wr32(ctl + 0x14, 1);
        for (uint32_t p = e.rd32(unit + 0xDC); p;) {
            const uint32_t next = e.rd32(p + 0x24);
            if (e.rd8(p) == 2) e.h(0x8E6F0, p);
            p = next;
        }
        return e.h(0x8EA80, 0, game, unit, 0, 0, 2, e.rd32(game + 0xA8) + 1, 0, 0, 0);
    }
    if (e.rd32(ctl + 0x14) == 4) {
        e.wr32(ctl + 0x14, 1);
        return gaiatk::Mode(e, game, unit, 0xA, 0, 0x105, 4);
    }
    e.h(0x9D230, 0, game, unit, 0, 0, ctl, e.rd32(ctl));
    return gaiatk::Idle(e, game, unit, 1);
}
}
