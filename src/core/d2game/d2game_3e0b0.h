// 1.13c D2Game+0x3E0B0: missile hit callback (table entry): a sub-missile burst, then an area hit with the
// missile's own damage. ecx = game, edx = the missile, one stack word (the unit that was hit, or 0; ret 4).
// 1 without a missile record or without a skill record for the missile's skill (0xB26A; level 0xB0FC);
// otherwise 3. The owner is 0xE03A0 with the owned bit, none without. With a sub-missile class (record short
// +0x24) of 0 or more: stats 0x38 and 0x86 of the missile are cleared (0xA472: unit, stat, 0, 0), the count is
// record +0x4C or, below 1, the calc at skill record +0x138 (0xAC9A: owner, calc, skill, level; at least 1),
// and 0x3D450 (game, missile, count, class, the 0xB240 value of (missile, owner, record +0x88, the missile's
// class, level)) throws them. The area range = record +0x50, or below 1 the calc at skill record +0x13C (at
// least 1). The damage block is zeroed and filled by 0x3A280 (the stack word; esi = block, edi = missile) and
// the record's hit flags are or-ed in. The missile's y is read before its x. With the owned bit and an owner
// (looked up again) the area hit runs (filter 0x8583, callback 0xA1210).
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3e0b0 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec) return 1;
    const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 1;
    const uint32_t owner = D2GameMissOwned(e, unit) ? D2GameMissOwner(e, game, unit) : 0;
    const int32_t sub = e.rd16s(rec + 0x24);
    if (sub >= 0) {
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 0x38, 0, 0);
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 0x86, 0, 0);
        int32_t count = (int32_t)e.rd32(rec + 0x4C);
        if (count <= 0) {
            count = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x138), skill, level);
            if (count < 1) count = 1;
        }
        const uint32_t v = e.h(0xB240, 0, 0, 0, 0, 0, unit, owner, e.rd32(rec + 0x88), e.rd32(unit + 4), level);
        e.x(0xB240, &level, 1);
        e.h(0x3D450, 0, 0, 0, 0, 0, game, unit, (uint32_t)count, (uint32_t)sub, v);
        e.x(0x3D450, &v, 1);
    }
    int32_t range = (int32_t)e.rd32(rec + 0x50);
    if (range <= 0) {
        range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x13C), skill, level);
        if (range < 1) range = 1;
    }
    uint32_t blk[28] = {};
    e.h(0x3A280, 0, 0, 0, 0, e.stk(blk), s0);
    e.edi(0x3A280, unit);
    D2GameMissHitFlags(e, rec, blk);
    const uint32_t y = D2GameUnitY(e, unit), x = D2GameUnitX(e, unit);
    if (!D2GameMissOwned(e, unit)) return 3;
    const uint32_t owner2 = D2GameMissOwner(e, game, unit);
    if (!owner2) return 3;
    D2GameAreaHit(e, game, owner2, x, y, (uint32_t)range, 0x8583, 0xA1210, blk, 28);
    return 3;
}
}
