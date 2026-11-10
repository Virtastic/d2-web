// 1.13c D2Game+0xF42D0: stdcall (game, unit, target, mode), ret 0x10, no result. target is read through the D2Common thunks 0xA478, 0xA556 (result unused), 0xA808 (the 0xA478 result; non-zero stops) and
// 0xAB1A(target, unit, 0x804) (non-zero stops); a null target is the fatal path (line 0x3F). v6 = 0xA42A(target, 6, 0): lo = max(v6 >> 5, 1), hi = max(v6 >> 3, lo + 1).
// vc = 0xA42A(target, 0xC, 0); byte = (roll(unit+0x20 seed, vc >> 2) + vc) & 0xFF, the roll being 0x11D0 (limit <= 0: 0 and the seed untouched; else seed = lo * 0x6AC690C5 + hi as 64 bits, result
// seed.lo % limit). chance = ((byte - 5 * (v2 >> 1) - vc) * 2 - v1f + 0x7D) floored at 0x41, v2 / v1f = 0xA42A(target, 2 / 0x1F, 0). One more seed step: a roll of
// (new seed.lo % 100) >= chance stops. Else 0xF4260(EAX = hi, EDI = lo, ECX = unit) == 0 stops. Else a 28-dword record: rec[2] = that value (mode 1: rec[4], rec[0x18] = 0x20;
// mode 2: rec[7], 0x40; mode 3: rec[9], 0x30), word rec+4 = 1 (| 4 when 0xA68E(target, 0x36) is zero); then 0xDC0B0(game, unit, target, rec), 0xDE0C0(EAX = target; game, unit, 0, rec), 0xDCF20(EAX = target; game, unit, rec).
#pragma once
#include <cstdint>
namespace gf42d0 {
template <class E, class F> uint32_t Run(E& e, F fatal, uint32_t game, uint32_t unit, uint32_t target, uint32_t mode)
{
    // 1.13c D2Game+0xF42D0
    const uint32_t t = e.h(0xA478, 0, 0, 0, 0, 0, target);
    e.h(0xA556, 0, 0, 0, 0, 0, t);   // its result is not used: 0xA808 takes the 0xA478 result again
    if (const uint32_t r = e.h(0xA808, 0, 0, 0, 0, 0, t)) return r;
    if (const uint32_t r = e.h(0xAB1A, 0, 0, 0, 0, 0, target, unit, 0x804)) return r;
    const uint32_t seed = unit + 0x20;
    if (!target) { fatal(0x3F); return 0; }
    const int32_t v6 = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, target, 6, 0);
    int32_t lo = v6 >> 5; if (lo < 1) lo = 1;
    int32_t hi = v6 >> 3; if (hi < lo + 1) hi = lo + 1;
    const int32_t vc = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, target, 0xC, 0);
    auto step = [&]() -> uint32_t {
        const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
        e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
        return (uint32_t)v;
    };
    const int32_t lim = vc >> 2;
    const uint32_t roll = lim > 0 ? step() % (uint32_t)lim : 0u;
    const uint32_t byte = (roll + (uint32_t)vc) & 0xFFu;
    const int32_t v2 = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, target, 2, 0);
    const int32_t v1f = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, target, 0x1F, 0);
    int32_t chance = (((int32_t)byte - 5 * (v2 >> 1) - vc) * 2) - v1f + 0x7D;
    if (chance < 0x41) chance = 0x41;
    const uint32_t last = step();
    if ((int32_t)(last % 100u) >= chance) return last / 100u;   // EAX keeps the quotient
    const uint32_t q = e.h(0xF4260, (uint32_t)hi, unit);
    e.j(0xF4260, (uint32_t)lo);
    if (!q) return 0;
    uint32_t rec[28] = {};
    switch (mode) {
    case 1: rec[4] = q; rec[0x18] = 0x20; break;
    case 2: rec[7] = q; rec[0x18] = 0x40; break;
    case 3: rec[9] = q; rec[0x18] = 0x30; break;
    default: rec[2] = q; break;
    }
    rec[1] = 1;
    if (!e.h(0xA68E, 0, 0, 0, 0, 0, target, 0x36)) rec[1] |= 4;
    e.h(0xDC0B0, 0, 0, 0, 0, 0, game, unit, target, e.stk(rec));
    e.blk(0xDC0B0, rec, 28);
    e.h(0xDE0C0, target, 0, 0, 0, 0, game, unit, 0, e.stk(rec));
    e.blk(0xDE0C0, rec, 28);
    const uint32_t r = e.h(0xDCF20, target, 0, 0, 0, 0, game, unit, e.stk(rec));
    e.blk(0xDCF20, rec, 28);
    return r;
}
}
