// 1.13c D2Game+0x98BA0: skill start callback (table entry) of a three-step melee skill with three element
// amounts. ecx = game, edx = the unit, two stack words (skill, level; ret 8). 0 without a skill record,
// without the unit's 0xAABA entry, or when the unit's target (0xA598 of its path, after 0xE07A0: eax = unit,
// ecx = game) is the unit itself or none. The damage block is zeroed; the hit roll 0xDE5A0 (eax = target, edi
// = unit; game, the 0xACD6 value of (unit, skill, level), 0) gives the low word of block word 1. With bit 0 of
// the roll: word 2 is 0xDC530 (eax = 0, ecx = 0, edx = unit; 1, 0, 0, 0, record byte +0x1A5); the record
// word +0x12E is or-ed into word 1 and word 0 is record +0x130 with bit 0; the weapon factor is 0x987D0 (esi
// = unit). Each of the calcs at record +0x138, +0x13C and +0x140 (0xAC9A: unit, calc, skill, level), when
// nonzero and the unit has stat 0x149, 0x14B or 0x14A (0xA42A: unit, stat, 0), grows by its share (0x14D0:
// eax = the stat, ecx = 100, edx = the calc); its share of the weapon factor (0x14D0: eax = the calc, ecx =
// 100, edx = the factor) is added to words 4, 9 and 7. Word 12 grows by 0xACE2 (unit, skill, level, 1). The
// entry's step counter (0xAD12) 0, 1 or 2 sets word 24 to 0x20, 0x30 or 0x40 and is advanced modulo 3
// (0xAC94). The attack is 0xDDDE0 (eax = unit, edi = block; game, target). 1.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g98ba0 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (target == unit || !target) return 0;
    uint32_t b[28] = {};
    const uint32_t q = e.h(0xACD6, 0, 0, 0, 0, 0, unit, skill, level);
    const uint32_t r = e.h(0xDE5A0, target, 0, 0, 0, 0, game, q, 0);
    e.edi(0xDE5A0, unit);
    b[1] = r & 0xFFFF;
    if (r & 1) {
        const uint32_t kind = e.rd8(sr + 0x1A5);
        b[2] = e.h(0xDC530, 0, 0, unit, 0, 0, 1, 0, 0, 0, kind);
        e.x(0xDC530, &kind, 1);
        b[1] |= e.rd32(sr + 0x12C) >> 16;
        b[0] = e.rd32(sr + 0x130) | 1;
        const uint32_t m = e.h(0x987D0, 0, 0, 0, 0, unit);
        static const uint32_t stat[3] = { 0x149, 0x14B, 0x14A }, word[3] = { 4, 9, 7 };
        for (int i = 0; i < 3; i++) {
            uint32_t v = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x138 + 4 * i), skill, level);
            if (v) {
                const uint32_t s = e.h(0xA42A, 0, 0, 0, 0, 0, unit, stat[i], 0);
                if (s) v += e.h(0x14D0, s, 100, v);
            }
            b[word[i]] += e.h(0x14D0, v, 100, m);
        }
        b[12] += e.h(0xACE2, 0, 0, 0, 0, 0, unit, skill, level, 1);
        const uint32_t n = e.h(0xAD12, 0, 0, 0, 0, 0, sk);
        if (n == 0) b[24] = 0x20;
        else if (n == 1) b[24] = 0x30;
        else if (n == 2) b[24] = 0x40;
        e.h(0xAC94, 0, 0, 0, 0, 0, sk, (uint32_t)((int32_t)(n + 1) % 3));
    }
    e.h(0xDDDE0, unit, 0, 0, 0, 0, game, target);
    e.blk(0xDDDE0, b, 28);
    return 1;
}
}
