// 1.13c D2Game+0x7D310: monster AI callback "maggot egg" (table entry). ecx = game, edx = unit, stack: the
// parameter block. With a skill in the record (word +0x170 not negative) and the control's word +0x14 not yet
// 1, a seed step modulo 100 below the difficulty word +0x5C hatches: the skill is used (0xF2380: eax = the
// skill with its low byte replaced by the record's byte +0x180, ecx = unit, ebx = game; stack: skill, target,
// 0, 0), the unit waits the difficulty word +0x56 (0xF20C0: eax = game, ebx = unit; stack: frames) and the
// control word becomes 1; that path leaves the control's address in EAX. A control word of 1 kills the unit:
// 0xE07A0 (eax = unit, ecx = game), then 0xDEEE0 (ecx = game, edx = unit; stack: the unit's owner from 0xA598
// (unit +0x2C), or 0 when that is the unit itself, 1). Then the unit idles the difficulty word +0x56.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace g7d310 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t row = e.rd32(tp + 0x1C), ctl = e.rd32(tp);
    bool die = false;
    if (e.rd16s(row + 0x170) >= 0) {
        if (e.rd32(ctl + 0x14) == 1) die = true;
        else if (gaiatk::Pct(e, unit) < gaiatk::Word(e, game, row, 0x5C)) {
            const uint32_t sk = (uint32_t)e.rd16s(row + 0x170);
            e.h(0xF2380, (sk & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, sk, e.rd32(tp + 8), 0, 0);
            e.h(0xF20C0, game, 0, 0, unit, 0, (uint32_t)gaiatk::Word(e, game, row, 0x56));
            e.wr32(ctl + 0x14, 1);
            return ctl;
        }
    }
    if (die || e.rd32(ctl + 0x14) == 1) {
        e.h(0xE07A0, unit, game);
        const uint32_t o = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
        e.h(0xDEEE0, 0, game, unit, 0, 0, o == unit ? 0 : o, 1);
    }
    return gaiatk::Idle(e, game, unit, (uint32_t)gaiatk::Word(e, game, row, 0x56));
}
}
