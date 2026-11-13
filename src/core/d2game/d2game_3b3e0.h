// 1.13c D2Game+0x3B3E0: missile hit callback (table entry): an area hit with the missile's own damage and an
// element amount. ecx = game, edx = the missile, one stack word (the unit that was hit, or 0; ret 4). Always
// 1. Needs a missile record and a skill record for the missile's skill (0xB26A; level 0xB0FC). Range = record
// +0x4C, or when that is below 1 the skill calc at skill record +0x64 run for the missile's owner (0xE0A20:
// eax = missile, game; 0xAC9A: owner, calc, skill, level), at least 1. The element amount = record +0x50, or
// when that is below 1 the calc at skill record +0x60 the same way (not clamped). The damage block is zeroed
// and filled by 0x3A280 (the stack word; esi = block, edi = missile); a positive amount is stored by 0x394B0
// (eax = missile, esi = amount, edi = block). The record's hit flags are or-ed in. With the owned bit and an
// owner (0xE03A0) the area hit runs at the missile's coordinates (filter 0x8583, callback 0xA1210).
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3b3e0 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec) return 1;
    const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 1;
    int32_t range = (int32_t)e.rd32(rec + 0x4C);
    if (range <= 0) {
        const uint32_t own = e.h(0xE0A20, unit, 0, 0, 0, 0, game);
        range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, own, e.rd32(sr + 0x64), skill, level);
        if (range < 1) range = 1;
    }
    int32_t amount = (int32_t)e.rd32(rec + 0x50);
    if (amount <= 0) {
        const uint32_t own = e.h(0xE0A20, unit, 0, 0, 0, 0, game);
        amount = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, own, e.rd32(sr + 0x60), skill, level);
    }
    const uint32_t x = D2GameUnitX(e, unit), y = D2GameUnitY(e, unit);
    uint32_t blk[28] = {};
    e.h(0x3A280, 0, 0, 0, 0, e.stk(blk), s0);
    e.edi(0x3A280, unit);
    if (amount > 0) {
        e.h(0x394B0, unit, 0, 0, 0, (uint32_t)amount);
        e.edi(0x394B0, e.stk(blk));
    }
    D2GameMissHitFlags(e, rec, blk);
    if (!D2GameMissOwned(e, unit)) return 1;
    const uint32_t owner = D2GameMissOwner(e, game, unit);
    if (!owner) return 1;
    D2GameAreaHit(e, game, owner, x, y, (uint32_t)range, 0x8583, 0xA1210, blk, 28);
    return 1;
}
}
