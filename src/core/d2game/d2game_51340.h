// 1.13c D2Game+0x51340: ECX = pGame, EDX = pUnit, stack = skill id, skill level (ret 8). Summons the unit's
// owner-linked pet anew. The rule over an environment E (HookEnv in the FN probe src/core/fn/d2game_51340.cpp): every callee is one E::h call in
// retail's order (disassembled).
// - record = skill table row when 0 <= id < count, else null; id 0 gives 0 (the test is on the id; retail then reads the record unguarded).
// - class byte = record +0xBE, 0 when not below data tables +0xBF0.
// - 0xE07A0 (EAX = unit, ECX = game); owner = (#thunk 0xA598)(unit +0x2C); owner == unit or null gives 0; 0x4EC30 (ESI = owner) zero gives 0.
// - 0x4F450 (ESI = unit, EBX = game); 0x50800 (EAX = owner, ECX = game); bExp = game +0x6A byte or game +0x74 nonzero.
// - lv = 0xA42A (owner, 0xC, 0); 0xAD7E (owner +4, bExp, game +0x6D difficulty, lv, 1, &range) fills range {lo, hi} (14 zeroed dwords);
//   hp = (0x11D0 (ECX = owner +0x20 seed, ESI = hi - lo + 1) + lo) << 8; stats 7 and 6 := hp (0xA472).
// - max = 0xA42A (unit, 0xC, 0), cur = 0xA42A (owner, 0xC, 0); cur nonzero and max < cur: v = max(1, 0x14D0 (EAX = max, ECX = cur,
//   EDX = hp)); stats 7, 6 := v, stat 0xC := max.
// - 0x4F970 (game, unit, owner, id, level, 0); 0x509E0 (EAX = unit; game, owner, id, level); c = 0xAC9A (unit, record +0xC0, id, level);
//   0x97F50 (EAX = class byte; game, unit, owner, c); 0xEFE40 (ECX = game, EBX = unit +0xD0, EDI = owner; 0); 0xA5DA (owner); 1.
#pragma once
#include <cstdint>

namespace g51340 {
template <class E> uint32_t SummonRefresh(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    const uint32_t rec = (id >= 0 && id < (int32_t)e.rd32(dt + 0xBA0)) ? (uint32_t)id * 0x23Cu + e.rd32(dt + 0xB98) : 0;
    if (id == 0) return 0;
    uint32_t cls = e.rd8(rec + 0xBE);
    if ((int32_t)cls >= (int32_t)e.rd32(dt + 0xBF0)) cls = 0;
    e.h(0xE07A0, unit, game);
    const uint32_t owner = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (owner == unit || !owner) return 0;
    if (!e.h(0x4EC30, 0, 0, 0, 0, owner)) return 0;
    e.h(0x4F450, 0, 0, 0, game, unit);
    e.h(0x50800, owner, game);
    const uint32_t bExp = (e.rd8(game + 0x6A) != 0 || e.rd32(game + 0x74) != 0) ? 1u : 0u;
    uint32_t z[14] = {};
    const uint32_t lv = e.h(0xA42A, 0, 0, 0, 0, 0, owner, 0xC, 0);
    const uint32_t w[6] = { e.rd32(owner + 4), bExp, e.rd8(game + 0x6D), lv, 1, e.stk(z) };
    e.h(0xAD7E, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5]);
    e.blk(0xAD7E, w, 6);
    const uint32_t hp = (e.h(0x11D0, 0, owner + 0x20, 0, 0, z[1] - z[0] + 1) + z[0]) << 8;
    e.h(0xA472, 0, 0, 0, 0, 0, owner, 7, hp, 0);
    e.h(0xA472, 0, 0, 0, 0, 0, owner, 6, hp, 0);
    const int32_t mx = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const int32_t cur = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, owner, 0xC, 0);
    if (cur != 0 && mx < cur) {
        int32_t v = (int32_t)e.h(0x14D0, (uint32_t)mx, (uint32_t)cur, hp);
        if (v < 1) v = 1;
        e.h(0xA472, 0, 0, 0, 0, 0, owner, 7, (uint32_t)v, 0);
        e.h(0xA472, 0, 0, 0, 0, 0, owner, 6, (uint32_t)v, 0);
        e.h(0xA472, 0, 0, 0, 0, 0, owner, 0xC, (uint32_t)mx, 0);
    }
    const uint32_t w2[6] = { game, unit, owner, (uint32_t)id, level, 0 };
    e.h(0x4F970, 0, 0, 0, 0, 0, w2[0], w2[1], w2[2], w2[3], w2[4], w2[5]);
    e.blk(0x4F970, w2, 6);
    e.h(0x509E0, unit, 0, 0, 0, 0, game, owner, (uint32_t)id, level);
    const uint32_t c = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), (uint32_t)id, level);
    e.h(0x97F50, cls, 0, 0, 0, 0, game, unit, owner, c);
    e.ed(owner);
    e.h(0xEFE40, 0, game, 0, e.rd32(unit + 0xD0), 0, 0);
    e.edi(0xEFE40, owner);
    e.h(0xA5DA, 0, 0, 0, 0, 0, owner);
    return 1;
}
}
