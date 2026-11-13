// 1.13c D2Game+0x39FD0: missile hit callback (table entry) of a healing missile. ecx = game, edx = the
// missile, one stack word (the unit that was hit; ret 4). 1 without a missile record or a hit unit. Skill
// 0xB26A, level 0xB0FC; the owner is 0xE03A0 with the owned bit, none without. With record +0x4C nonzero and
// an owner, the hit unit is healed when 0xE0210 (eax = hit unit, ecx = skill, esi = owner) passes or 0xE0660
// (eax = hit unit; game, owner, skill) does: with the skill record (0x1420, eax = skill; none: 1) the amount
// is a roll (0x11D0: ecx = the missile's seed at +0x20, esi = max - min) over the calcs at skill record
// +0x138 and +0x13C (0xAC9A: owner, calc, skill, level; each times 256) plus the minimum; an amount below 1
// is replaced by 0x39B40 (eax = 0x16, ecx = -1; missile, 0x15). Stat 6 of the hit unit (0xA42A: unit, 6, 0)
// plus the amount, capped at 0xA3A0 of the unit, is stored (0xA472: unit, 6, value, 0); a positive record
// short +0x36 goes to 0xA83E (unit, value, 0). 1. Otherwise the result depends on the hit unit's type: type 0
// gives 3 when record +0x50 is zero and 4 when not; type 1 gives 3 when record +0x50 is zero, or is 2 and
// 0xA814 of the unit passes, or is anything else and 0xA508 passes, else 4; other types 4.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g39fd0 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t target)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec || !target) return 1;
    const uint32_t skill = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t level = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t owner = D2GameMissOwned(e, unit) ? D2GameMissOwner(e, game, unit) : 0;
    if (e.rd32(rec + 0x4C) && owner
        && (e.h(0xE0210, target, skill, 0, 0, owner) || e.h(0xE0660, target, 0, 0, 0, 0, game, owner, skill))) {
        const uint32_t sr = e.h(0x1420, skill);
        if (!sr) return 1;
        const int32_t lo = (int32_t)(e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x138), skill, level) << 8);
        const int32_t hi = (int32_t)(e.h(0xAC9A, 0, 0, 0, 0, 0, owner, e.rd32(sr + 0x13C), skill, level) << 8);
        int32_t amount = (int32_t)(e.h(0x11D0, 0, unit + 0x20, 0, 0, (uint32_t)(hi - lo)) + (uint32_t)lo);
        if (amount <= 0) amount = (int32_t)e.h(0x39B40, 0x16, 0xFFFFFFFFu, 0, 0, 0, unit, 0x15);
        int32_t life = (int32_t)(e.h(0xA42A, 0, 0, 0, 0, 0, target, 6, 0) + (uint32_t)amount);
        const int32_t cap = (int32_t)e.h(0xA3A0, 0, 0, 0, 0, 0, target);
        if (life >= cap) life = cap;
        e.h(0xA472, 0, 0, 0, 0, 0, target, 6, (uint32_t)life, 0);
        const int32_t st = e.rd16s(rec + 0x36);
        if (st > 0) e.h(0xA83E, 0, 0, 0, 0, 0, target, (uint32_t)st, 0);
        return 1;
    }
    const uint32_t type = e.rd32(target), mode = e.rd32(rec + 0x50);
    if (type == 0) return mode ? 4 : 3;
    if (type != 1) return 4;
    if (mode == 0) return 3;
    if (mode == 2) return e.h(0xA814, 0, 0, 0, 0, 0, target) ? 3 : 4;
    return e.h(0xA508, 0, 0, 0, 0, 0, target) ? 3 : 4;
}
}
