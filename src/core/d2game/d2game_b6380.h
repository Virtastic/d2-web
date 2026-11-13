// 1.13c D2Game+0xB6380: skill start callback (table entry) of a strike that carries the unit past its
// target. ecx = game, edx = the unit, two stack words (skill, level; ret 8). 0 without a skill record,
// without the unit's 0xAABA entry, without a path, or when 0xA68E (unit, 1) is set. After 0xE07A0 (eax =
// unit, ecx = game) the target is 0xA598 of the path, the unit itself counting as none. Without a target the
// destination is the path's target point (0xAAC6, 0xAAD8) and the entry's target id is cleared (0xAB20:
// entry, -1); with one it is the unit's position mirrored through the target (twice the target's 0x1100 and
// 0x1130 less the unit's). The room at the destination is 0x1280 (eax = the unit's room 0xA478, ebx = x, edi
// = y); none returns 0. With a target the damage block is zeroed, the hit roll 0xDE5A0 (eax = target, edi =
// unit; game, the 0xACD6 value of (unit, skill, level), 0) gives the low word of block word 1, and with bit 0
// of it the record word +0x12E is or-ed into word 1, record +0x130 into word 0, a nonzero record +0x134
// becomes word 24, word 3 is the calc at record +0x138 (0xAC9A: unit, calc, skill, level) and 0x9F210 (eax =
// skill, edi = unit; block, level) runs; the attack is 0xDDDE0 (eax = unit, edi = block; game, target) and
// the entry keeps the target's type (0xAD0C) and id (0xAB20). Without a target the id is cleared again.
// Then 0xAD2A (the skill record, read from its stack slot where the room would be expected; x, y, the 0xAD00 value of (unit, 0x100)), 0xE2240 (ecx = unit, edx = 1), and the entry
// keeps x (0xAC94), y (0xACDC) and the flag 0x80 (0xA78A). 1.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace gb6380 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    if (!e.rd32(unit + 0x2C)) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 1)) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    const uint32_t target = t == unit ? 0 : t;
    uint32_t x, y;
    if (!target) {
        x = e.h(0xAAC6, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
        y = e.h(0xAAD8, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
        e.h(0xAB20, 0, 0, 0, 0, 0, sk, 0xFFFFFFFFu);
    } else {
        const uint32_t tx = e.h(0x1100, 0, target), ty = e.h(0x1130, 0, target);
        x = 2 * tx - e.h(0x1100, 0, unit);
        y = 2 * ty - e.h(0x1130, 0, unit);
    }
    const uint32_t room = e.h(0x1280, e.h(0xA478, 0, 0, 0, 0, 0, unit), 0, 0, x);
    e.edi(0x1280, y);
    if (!room) return 0;
    if (target) {
        uint32_t b[28] = {};
        const uint32_t q = e.h(0xACD6, 0, 0, 0, 0, 0, unit, skill, level);
        const uint32_t r = e.h(0xDE5A0, target, 0, 0, 0, 0, game, q, 0);
        e.edi(0xDE5A0, unit);
        b[1] = r & 0xFFFF;
        if (r & 1) {
            b[1] |= e.rd32(sr + 0x12C) >> 16;
            b[0] |= e.rd32(sr + 0x130);
            if (e.rd32(sr + 0x134)) b[24] = e.rd32(sr + 0x134);
            b[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x138), skill, level);
            e.h(0x9F210, skill, 0, 0, 0, 0, e.stk(b), level);
            e.edi(0x9F210, unit);
        }
        e.h(0xDDDE0, unit, 0, 0, 0, 0, game, target);
        e.blk(0xDDDE0, b, 28);
        e.h(0xAD0C, 0, 0, 0, 0, 0, sk, e.rd32(target));
        e.h(0xAB20, 0, 0, 0, 0, 0, sk, e.rd32(target + 0xC));
    } else {
        e.h(0xAB20, 0, 0, 0, 0, 0, sk, 0xFFFFFFFFu);
    }
    const uint32_t w = e.h(0xAD00, 0, 0, 0, 0, 0, unit, 0x100);
    e.h(0xAD2A, 0, 0, 0, 0, 0, sr, x, y, w);   // retail pushes the skill record's slot ([esp+0x20]), not the room's
    e.h(0xE2240, 0, unit, 1);
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, x);
    e.h(0xACDC, 0, 0, 0, 0, 0, sk, y);
    e.h(0xA78A, 0, 0, 0, 0, 0, sk, 0x80);
    return 1;
}
}
