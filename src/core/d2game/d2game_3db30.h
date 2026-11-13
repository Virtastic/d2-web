// 1.13c D2Game+0x3DB30: missile hit callback (table entry) of a delayed strike on a tracked target. ecx =
// game, edx = the missile, one stack word (unread; ret 4). 1 without a missile record, with a negative
// record short +0x24, without the owned bit, without an owner (0xE03A0), or when the missile's skill (0xB26A;
// its level 0xB0FC) has no skill record. The target is 0xE03A0 (eax = 0xB24C of the missile, ecx = game,
// edx = 0xB288 of the missile); none: 0. Then position 0x1100 / 0x1130 (ecx = missile); range = record +0x4C,
// or when that is below 1 the skill calc at skill record +0x64 (0xAC9A: owner, calc, skill, level), at least
// 1; the same for record +0x50 with the calc at +0x144. The damage block is filled by 0x3A4E0 (target, block)
// and adjusted by 0x3AFF0 (eax = game, ecx = block, ebx = missile, esi = target). The area hit runs around
// the position with the skill record's filter (+0x50, 0xA683 when zero), the callback 0x3CC30 and a six-word
// argument {owner, missile, skill, level, the +0x50 value, the +0x24 class} that the damage block follows. 1.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3db30 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec || e.rd16s(rec + 0x24) < 0 || !D2GameMissOwned(e, unit)) return 1;
    const uint32_t owner = D2GameMissOwner(e, game, unit);
    if (!owner) return 1;
    const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 1;
    const uint32_t tid = e.h(0xB288, 0, 0, 0, 0, 0, unit);
    const uint32_t ttype = e.h(0xB24C, 0, 0, 0, 0, 0, unit);
    const uint32_t target = e.h(0xE03A0, ttype, game, tid);
    if (!target) return 0;
    const uint32_t x = e.h(0x1100, 0, unit);
    const uint32_t y = e.h(0x1130, 0, unit);
    int32_t range = (int32_t)e.rd32(rec + 0x4C);
    if (range <= 0) {
        range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x64), skill, level);
        if (range < 1) range = 1;
    }
    int32_t v = (int32_t)e.rd32(rec + 0x50);
    if (v <= 0) {
        v = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x144), skill, level);
        if (v < 1) v = 1;
    }
    uint32_t a[6 + 28];
    e.h(0x3A4E0, 0, 0, 0, 0, 0, target, e.stk(a + 6));
    e.h(0x3AFF0, game, e.stk(a + 6), 0, unit, target);
    uint32_t flags = e.rd32(sr + 0x50);
    if (!flags) flags = 0xA683;
    a[0] = owner; a[1] = unit; a[2] = skill; a[3] = level; a[4] = (uint32_t)v; a[5] = (uint32_t)e.rd16s(rec + 0x24);
    D2GameAreaHit(e, game, owner, x, y, (uint32_t)range, flags, 0x3CC30, a, 34);
    return 1;
}
}
