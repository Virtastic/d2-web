// 1.13c D2Game+0x4F090 -- on-hit transfer to a monster of class 0x122: ECX = game, stdcall words: attacker, victim unit, damage record (word +8 > 0), skill id, level (negative becomes 1).
// Attacker type 0 or 1 and not 0xDFA90; skill row (0x1420(skill id)) short +0x82 selects the 0xA5D4 record (0xA68E must hold). The record's owner (0xE03A0 over 0xA7FC, game, 0xA84A) feeds
// 0xAC9A(owner, row +0x138 | +0x13C | +0x140 [by 0xA7F6 / victim type], skill id, level); amount = 0x14D0(that, 100, record +8). A 28-word block (word 0 |= row +0x130, word 1 low |= row +0x12E | 0x20,
// word 2 = amount, word 24 = row +0x134) goes to 0xDE0C0(attacker; game, victim, 1, block) and 0xDCF20(attacker; game, victim, block). A type-1 victim of class 0x122 (not 0xA514) then
// takes a percent from the byte at 0x1540(victim) + game +0x6D + 0xA0 (zero ends); the amount is scaled by it unless 100, 0xDFA90 on the owner (0x21260(attacker)) and attacker end it,
// then 0x14D0(20, 100, .) must stay positive; the owner (when any) takes 50 percent via 0x4DF20, the attacker the remainder, the owner what is left; 0xA83E(unit, 0x97, 0) on both. Returns 1 past the skill row, else 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f4f090(NE& e, uint32_t dt, uint32_t game, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    if (!a2 || !a1) return 0;
    const uint32_t t = e.rd32(a1);
    if (t != 0 && t != 1) return 0;
    if (e.h(0xDFA90, a1)) return 0;
    if (!a3) return 0;
    if ((int32_t)e.rd32(a3 + 8) <= 0) return 0;
    int32_t lvl = (int32_t)a5;
    const uint32_t row = e.h(0x1420, a4);
    if (!row) return 0;
    if (lvl < 0) lvl = 1;
    const int32_t s82 = (int16_t)e.rd16(row + 0x82);
    if (s82 < 0 || s82 >= (int32_t)e.rd32(dt + 0xc4)) return 0;
    if (!e.imp(0xA68E, a1, (uint32_t)s82)) return 0;
    const uint32_t rec = e.imp(0xA5D4, a1, (uint32_t)s82);
    if (!rec) return 0;
    const uint32_t v1 = e.imp(0xA84A, rec);
    const uint32_t v2 = e.imp(0xA7FC, rec);
    const uint32_t own = e.h(0xE03A0, v2, game, v1);
    uint32_t field;
    if (e.rd32(a1) != 0 && e.imp(0xA7F6, a1) == 0) field = e.rd32(row + 0x138);
    else if (e.rd32(a2) == 0) field = e.rd32(row + 0x13c);
    else field = e.rd32(row + 0x140);
    const uint32_t x = e.imp(0xAC9A, own, field, a4, (uint32_t)lvl);
    uint32_t blk[28] = {};
    blk[2] = e.h(0x14D0, x, 0x64, e.rd32(a3 + 8));
    blk[1] |= (e.rd16(row + 0x12e) | 0x20u) & 0xFFFFu;
    blk[0] |= e.rd32(row + 0x130);
    blk[24] = e.rd32(row + 0x134);
    e.h(0xDE0C0, a1, 0, 0, 0, 0, game, a2, 1, e.stk(blk));
    e.blk(0xDE0C0, blk, 28);
    e.h(0xDCF20, a1, 0, 0, 0, 0, game, a2, e.stk(blk));
    e.blk(0xDCF20, blk, 28);
    if (e.rd32(a2) != 1) return 1;
    if (e.rd32(a2 + 4) != 0x122) return 1;
    const uint32_t q = e.h(0x1540, a2);
    if (!q) return 1;
    const uint32_t pct = e.rd8(q + e.rd8(game + 0x6d) + 0xa0);
    if (!pct) return 1;
    if (e.imp(0xA514, a2)) return 1;
    int32_t amt = (int32_t)blk[2];
    if (pct != 0x64) amt = (int32_t)e.h(0x14D0, pct, 0x64, (uint32_t)amt);
    const uint32_t o = e.h(0x21260, 0, a1);
    if (o && e.h(0xDFA90, o)) return 1;
    if (e.h(0xDFA90, a1)) return 1;
    amt = (int32_t)e.h(0x14D0, 0x14, 0x64, (uint32_t)amt);
    if (amt <= 0) return 1;
    if (o) {
        const uint32_t h = e.h(0x14D0, 0x32, 0x64, (uint32_t)amt);
        amt -= (int32_t)e.h(0x4DF20, h, 0, 0, o);
    }
    amt -= (int32_t)e.h(0x4DF20, (uint32_t)amt, 0, 0, a1);
    if (o && amt > 0) e.h(0x4DF20, (uint32_t)amt, 0, 0, o);
    e.imp(0xA83E, a1, 0x97, 0);
    if (o) e.imp(0xA83E, o, 0x97, 0);
    return 1;
}
}
