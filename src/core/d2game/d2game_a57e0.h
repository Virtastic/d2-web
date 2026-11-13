// 1.13c D2Game+0xA57E0: monster AI callback "hydra" (table entry). ecx = game, edx = unit, stack: the
// parameter block. Once the game frame (+0xA8) is past the control's word +0x14 the unit dies (mode 0). Before
// that, with a target nearer than 25 and a seed step modulo 100 below 60, it uses the record's skill (word
// +0x170) on the target (0xF2380: eax = the target with its low byte replaced by the record's byte +0x180,
// ecx = unit, ebx = game; stack: skill, target, 0, 0); otherwise it idles 10 frames.
#pragma once
#include <cstdint>
#include "d2game_aiatk.h"

namespace ga57e0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    const uint32_t ctl = e.rd32(tp), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    if ((int32_t)e.rd32(ctl + 0x14) < (int32_t)e.rd32(game + 0xA8)) return gaiatk::Mode(e, game, unit, 0, 0);
    if (tgt && (int32_t)e.rd32(tp + 0x14) < 0x19 && gaiatk::Pct(e, unit) < 0x3C)
        return e.h(0xF2380, (tgt & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), tgt, 0, 0);
    return gaiatk::Idle(e, game, unit, 0xA);
}
}
