// 1.13c D2Game+0x3B210: missile hit callback (table entry): an area hit and a sub-missile at the missile's
// place. ecx = game, edx = the missile, one stack word (unread; ret 4). Always 1. Needs a missile record, the
// owned bit and an owner (0xE03A0). Skill 0xB26A, level 0xB0FC, the missile's coordinates, and the damage
// block from 0x3A4E0 (0, block). Range = record +0x4C, or when that is below 1 the skill calc at skill record
// +0x64 (0xAC9A: owner, calc, skill, level; no skill record ends the call), at least 1. The record's hit
// flags are or-ed into the block and the area hit runs (filter 0x8583, callback 0xA1210). Then, with a
// sub-missile class (record short +0x24) of 0 or more, 0xA0000 (esi = owner; game, skill, level, class, x, y).
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3b210 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec || !D2GameMissOwned(e, unit)) return 1;
    const uint32_t owner = D2GameMissOwner(e, game, unit);
    if (!owner) return 1;
    const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t x = D2GameUnitX(e, unit), y = D2GameUnitY(e, unit);
    uint32_t blk[28];
    e.h(0x3A4E0, 0, 0, 0, 0, 0, 0, e.stk(blk));
    int32_t range = (int32_t)e.rd32(rec + 0x4C);
    if (range <= 0) {
        const uint32_t sr = D2GameSkillRec(e, dt, skill);
        if (!sr) return 1;
        range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x64), skill, level);
        if (range < 1) range = 1;
    }
    D2GameMissHitFlags(e, rec, blk);
    D2GameAreaHit(e, game, owner, x, y, (uint32_t)range, 0x8583, 0xA1210, blk, 28);
    const int32_t sub = e.rd16s(rec + 0x24);
    if (sub >= 0) {
        const uint32_t w[2] = { x, y };
        e.h(0xA0000, 0, 0, 0, 0, owner, game, skill, level, (uint32_t)sub, x, y);
        e.x(0xA0000, w, 2);
    }
    return 1;
}
}
