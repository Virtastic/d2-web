// 1.13c D2Game+0x4C3A0: skill start callback (table entry) of a melee skill with a rolled damage and an
// optional timed effect. ecx = game, edx = the unit, two stack words (skill, level; ret 8). 0 without a skill
// record, without 0xAABA of the unit, or when the unit's target (0xA598 of its path, after 0xE07A0: eax =
// unit, ecx = game) is the unit itself or none. The damage block is zeroed; the hit roll 0xDE5A0 (eax =
// target, edi = unit; game, 0, 0) gives the low word of block word 1. With bit 0 of the roll: word 7 is a
// random amount (0x11D0: ecx = the unit's seed at +0x20, esi = max - min) between 0xAE92 and 0xAEC2 (unit,
// skill, level, 1) plus the minimum; word 3 is the calc at record +0x138 (0xAC9A: unit, calc, skill, level);
// with a nonzero record byte +0x1DC, word 26 is the calc at +0x144 and, when that is above 0, byte 1 of word
// 25 is the record byte. The attack is 0xDDDE0 (eax = unit, edi = block; game, target). 1.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g4c3a0 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    if (!e.h(0xAABA, 0, 0, 0, 0, 0, unit)) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (target == unit || !target) return 0;
    uint32_t b[28] = {};
    const uint32_t r = e.h(0xDE5A0, target, 0, 0, 0, 0, game, 0, 0);
    e.edi(0xDE5A0, unit);
    b[1] = r & 0xFFFF;
    if (r & 1) {
        const uint32_t lo = e.h(0xAE92, 0, 0, 0, 0, 0, unit, skill, level, 1);
        const uint32_t hi = e.h(0xAEC2, 0, 0, 0, 0, 0, unit, skill, level, 1);
        b[7] = e.h(0x11D0, 0, unit + 0x20, 0, 0, hi - lo) + lo;
        b[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x138), skill, level);
        if (e.rd8(sr + 0x1DC)) {
            const uint32_t t = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x144), skill, level);
            b[26] = t;
            if ((int32_t)t > 0) b[25] = (b[25] & ~0xFF00u) | (e.rd8(sr + 0x1DC) << 8);
        }
    }
    e.h(0xDDDE0, unit, 0, 0, 0, 0, game, target);
    e.blk(0xDDDE0, b, 28);
    return 1;
}
}
