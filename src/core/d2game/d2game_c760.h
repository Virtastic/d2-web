// 1.13c D2Game+0xC760: skill start callback (table entry) of a melee skill with three bonus calcs. ecx = game,
// edx = the unit, two stack words (skill, level; ret 8). 0 without a skill record, or when the unit's target
// (0xA598 of its path, after 0xE07A0: eax = unit, ecx = game) is the unit itself or none. The zeroed damage
// block is filled by 0xA11B0 (eax = 0, ecx = level, edx = skill, ebx = skill record, esi = block; game, unit,
// target, 0); without bit 0 of block word 1 the call ends with 1. Block word 0 gets bit 1 and the record's
// dword at +0x130; a nonzero record +0x134 becomes word 24. 0xDCE00 (eax = unit, ebx = level, esi = skill;
// block) runs, then the calc at record +0x138 (0xAC9A: unit, calc, skill, level) is turned into a share of
// word 2 (0x14D0: eax = calc, ecx = 100, edx = word 2) and added to it; the calcs at +0x13C and +0x140 are
// added to words 14 and 15. The attack is 0xDDDE0 (eax = unit, edi = block; game, target). 1.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace gc760 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (target == unit || !target) return 0;
    uint32_t b[28] = {};
    e.h(0xA11B0, 0, level, skill, sr, e.stk(b), game, unit, target, 0);
    if (!(b[1] & 1)) return 1;
    b[0] |= 2 | e.rd32(sr + 0x130);
    if (e.rd32(sr + 0x134)) b[24] = e.rd32(sr + 0x134);
    e.h(0xDCE00, unit, 0, 0, level, skill, e.stk(b));
    const uint32_t r1 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x138), skill, level);
    b[2] += e.h(0x14D0, r1, 100, b[2]);
    const uint32_t r2 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x13C), skill, level);
    const uint32_t r3 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x140), skill, level);
    b[14] += r2;
    b[15] += r3;
    e.h(0xDDDE0, unit, 0, 0, 0, 0, game, target);
    e.blk(0xDDDE0, b, 28);
    return 1;
}
}
