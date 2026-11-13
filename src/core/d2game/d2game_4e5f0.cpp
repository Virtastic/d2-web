// 1.13c D2Game+0x4E5F0 -- life-steal style transfer for a hit: ECX = game, stdcall words: attacker, victim unit, damage record (word +8 = amount), skill id, level.
// Needs all three pointers and unit types (word +0) 0 or 1. A monster victim (type 1) takes its percent from the byte at 0x1540(victim) + difficulty (game +0x6D) + 0xA0 (0 ends it);
// the percent scales the amount (0x14D0(percent, 100, amount) unless 100). Ends with 0 when 0xA514(victim) is set. amount = min(amount, 0xA42A(victim, 6, 0)); 0xDFA90 of the
// attacker's owner (0x21260(attacker), when any) or of the attacker ends it; amount = 0x14D0(0xB1B0(level, skill id), 100, amount) must stay positive. The attacker's skill-row value
// 0x1BF0(skill id) feeds 0x4DF20 for the owner (0x14D0 scaled), the remainder goes to the attacker, then to the owner; 0xA83E(unit, 0x97, 0) clears the state on the attacker and on
// the owner (value positive). Returns 1 on a transfer, else 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f4e5f0(NE& e, uint32_t game, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5)
{
    if (!p1 || !p2 || !p3) return 0;
    const uint32_t t = e.rd32(p2);
    if (t != 0 && t != 1) return 0;
    uint32_t pct = 0x64;
    if (t == 1) {
        const uint32_t q = e.h(0x1540, p2);
        if (!q) return 0;
        pct = e.rd8(q + e.rd8(game + 0x6d) + 0xa0);
        if (!pct) return 0;
    }
    if (e.imp(0xA514, p2)) return 0;
    int32_t amt = (int32_t)e.rd32(p3 + 8);
    if (pct != 0x64) amt = (int32_t)e.h(0x14D0, pct, 100, (uint32_t)amt);
    const int32_t cap = (int32_t)e.imp(0xA42A, p2, 6, 0);
    if (amt >= cap) amt = cap;
    const uint32_t o = e.h(0x21260, 0, p1);
    if (o && e.h(0xDFA90, o)) return 0;
    if (e.h(0xDFA90, p1)) return 0;
    const int32_t lv = (int32_t)e.imp(0xB1B0, p5, p4);
    if (lv <= 0) return 0;
    amt = (int32_t)e.h(0x14D0, (uint32_t)lv, 100, (uint32_t)amt);
    if (amt <= 0) return 0;
    const int32_t sv = (int32_t)e.h(0x1BF0, p4);
    if (o && sv > 0) {
        const uint32_t v = e.h(0x14D0, (uint32_t)sv, 100, (uint32_t)amt);
        amt -= (int32_t)e.h(0x4DF20, v, 0, 0, o);
    }
    amt -= (int32_t)e.h(0x4DF20, (uint32_t)amt, 0, 0, p1);
    if (o && amt > 0) e.h(0x4DF20, (uint32_t)amt, 0, 0, o);
    e.imp(0xA83E, p1, 0x97, 0);
    if (sv > 0 && o) e.imp(0xA83E, o, 0x97, 0);
    return 1;
}
}
