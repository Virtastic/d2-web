// 1.13c D2Game+0xCF00 (blood-drain skill: the caster heals by part of its target's life): ecx = pGame, edx = pUnit,
// stack = (skill id, skill level), ret 8. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a skill id outside the table or a null record returns 0. 0xE07A0 (eax = unit, ecx = game) runs first; the target
// is 0xA598(unit+0x2C); none, or the unit itself, returns 0. The unit's flags (+0xC4) |= 0x40. 0xDAA90 (esi = unit,
// edx = target, keeps ecx/edx) gives the drain entry; none returns 0. Entry byte +4 bit 0 set: the healed unit is the
// player's owner (0x21260, ecx = unit; none: the unit itself) for a player (type 1), else null; its life (0xA42A stat 6)
// and max life (0xA3A0) are read, the target's life too; the drained amount is the entry's cap (+8) when that is
// below the target's life, else the target's life; 0x14D0 (eax = percent from 0xAC9A(unit, rec+0x138, id, level),
// edx = amount, ecx = 100) scales it; life + that, at least 1, at most max life, is written back (0xA472 stat 6).
// Both paths end with 0xDED30(game, unit, target). Result 1.
#pragma once
#include <cstdint>

namespace cf00 {
template <class E> uint32_t SuckBlood(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t ent = e.h(0xDAA90, 0, 0, tgt, 0, unit);
    if (!ent) return 0;
    if (e.rd8(ent + 4) & 1) {
        uint32_t who = 0;
        if (e.rd32(unit) == 1) { who = e.h(0x21260, 0, unit); if (!who) who = unit; }
        int32_t hp = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, who, 6, 0);
        const int32_t mx = (int32_t)e.h(0xA3A0, 0, 0, 0, 0, 0, who);
        const int32_t t = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, 6, 0);
        const int32_t cap = (int32_t)e.rd32(ent + 8);
        const int32_t amt = cap < t ? cap : t;
        const uint32_t pct = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skillId, level);
        hp += (int32_t)e.h(0x14D0, pct, 100, (uint32_t)amt);
        if (hp < 1) hp = 1;
        e.h(0xA472, 0, 0, 0, 0, 0, who, 6, (uint32_t)(hp < mx ? hp : mx), 0);
    }
    e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
    return 1;
}
}
