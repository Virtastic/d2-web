// 1.13c D2Game+0x3B5F0: missile hit callback (table entry): an area hit around the missile. ecx = game, edx =
// the missile, one stack word (the unit that was hit, or 0; ret 4). Always 1. Needs a missile record; the
// owner is 0xE03A0 with the owned bit, none without. Range = record +0x4C, or when that is exactly zero the
// skill calc at skill record +0x64 (0xAC9A: owner, calc, skill 0xB26A, level 0xB0FC; no skill record ends
// the call), at least 1. The damage block is zeroed and filled by 0x3A4E0 (the stack word, block) and the
// record's hit flags are or-ed in; only with an owner does the area hit run at the missile's coordinates
// (filter 0x8583, callback 0xA1210).
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3b5f0 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec) return 1;
    const uint32_t owner = D2GameMissOwned(e, unit) ? D2GameMissOwner(e, game, unit) : 0;
    int32_t range = (int32_t)e.rd32(rec + 0x4C);
    if (range == 0) {
        const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
        const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
        const uint32_t sr = D2GameSkillRec(e, dt, skill);
        if (!sr) return 1;
        range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x64), skill, level);
        if (range < 1) range = 1;
    }
    const uint32_t x = D2GameUnitX(e, unit), y = D2GameUnitY(e, unit);
    uint32_t blk[28] = {};
    e.h(0x3A4E0, 0, 0, 0, 0, 0, s0, e.stk(blk));
    D2GameMissHitFlags(e, rec, blk);
    if (!owner) return 1;
    D2GameAreaHit(e, game, owner, x, y, (uint32_t)range, 0x8583, 0xA1210, blk, 28);
    return 1;
}
}
