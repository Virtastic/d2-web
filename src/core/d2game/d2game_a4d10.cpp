// 1.13c D2Game+0xA4D10 -- one step of a monster's attack cooldown. ECX = game, EDX = rec (the monster), stack ai (ret 4).
// rec must be set with bit 10 of rec+0xC8, its target D2Game+0xE03A0 (EAX = rec+0x94 type, ECX = game, EDX = rec+0x98 id)
// must exist and D2Game+0xDFA90 (EAX = target) must say it is not dead; otherwise D2Game+0xDEEE0 (ECX = game, EDX = rec;
// 0, 1) ends the step. st = *ai, tbl = ai+0x1C (a monstats row), i = the difficulty byte game+0x6D. A zero deadline st+0x1C
// becomes game+0xA8 (frame) + (rand(tbl.s[0x62+2i]) + tbl.s[0x62+2i]) * 25, rand being D2Game+0x1180 (EAX = range,
// ECX = the seed at rec+0x20). frame > deadline (signed): DEEE0. Else, with ai+0x18 set and rand(100) < tbl.s[0x56+2i],
// D2Game+0xF2220 (EAX = 4, EDI = rec; game, ai+8); else D2Game+0xF22C0 (EAX = game, ESI = rec; tbl.s[0x5C+2i]).
// The function is void; EAX is left as the last callee returned it.
#pragma once
#include <cstdint>
namespace ga4d10 {
// 1.13c D2Game+0xA4D10
template <class E>
uint32_t Step(E& e, uint32_t game, uint32_t rec, uint32_t ai)
{
    if (rec && ((E::rd32(rec + 0xC8) >> 10) & 1)) {
        const uint32_t tgt = e.h(0xE03A0, E::rd32(rec + 0x94), game, E::rd32(rec + 0x98));
        if (tgt && e.h(0xDFA90, tgt) == 0) {
            const uint32_t flag = E::rd32(ai + 0x18), aux = E::rd32(ai + 8), tbl = E::rd32(ai + 0x1C);
            const uint32_t i2 = (uint32_t)E::rd8(game + 0x6D) * 2;
            if (E::rd32(E::rd32(ai) + 0x1C) == 0) {
                const int32_t base = E::rd16s(tbl + 0x62 + i2);
                const uint32_t r = e.h(0x1180, (uint32_t)base, rec + 0x20);
                E::wr32(E::rd32(ai) + 0x1C, E::rd32(game + 0xA8) + (r + (uint32_t)base) * 0x19);
            }
            if ((int32_t)E::rd32(game + 0xA8) > (int32_t)E::rd32(E::rd32(ai) + 0x1C))
                return e.h(0xDEEE0, 0, game, rec, 0, 0, 0, 1);
            if (flag && (int32_t)e.h(0x1180, 100, rec + 0x20) < E::rd16s(tbl + 0x56 + i2)) {
                const uint32_t v = e.h(0xF2220, 4, 0, 0, 0, 0, game, aux);
                e.edi(0xF2220, rec);
                return v;
            }
            return e.h(0xF22C0, game, 0, 0, 0, rec, (uint32_t)E::rd16s(tbl + 0x5C + i2));
        }
    }
    return e.h(0xDEEE0, 0, game, rec, 0, 0, 0, 1);
}
}
