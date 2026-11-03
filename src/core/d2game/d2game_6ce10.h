// 1.13c D2Game+0x6CE10 (AITHINK_IdleRoll90): ecx = think context {+0 game, +4 unit, +0xC seed pointer, +0x10 monster id}, plain ret, returns 0.
// Rule: a non-null unit with +0x10 (mode) != 0 does nothing. Otherwise the record of the unit's +4 (or -1 for no unit) is fetched (0xA520);
// the seed steps (v = lo * 0x6AC690C5 + hi, lo = v, hi = v >> 32) and r = lo % 100. r < 90: 0x6B570(ctx, 0) and, when it is non-zero,
// mode 5 (0xA4A2). Otherwise mode 1, a mode-1 frame send at (record+0xDC >> 8) + 1 + frame, then a mode-9 send at frame + 0x14 (0x8EA80).
#pragma once
#include <cstdint>

namespace r6ce10 {
template <class E> uint32_t Roll(E& e, uint32_t seed, uint32_t mod)
{
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v);
    e.wr32(seed + 4, (uint32_t)(v >> 32));
    return (uint32_t)v % mod;
}
template <class E> uint32_t IdleRoll90(E& e, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 0;
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, unit ? e.rd32(unit + 4) : 0xFFFFFFFFu);
    const uint32_t r = Roll(e, e.rd32(ctx + 0xC), 100);
    if (r < 0x5A) {
        if (e.h(0x6B570, 0, 0, 0, 0, 0, ctx, 0)) e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 5);
        return 0;
    }
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 1);
    const uint32_t game = e.rd32(ctx);
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    e.h(0x8EA80, 0, e.rd32(ctx), e.rd32(ctx + 4), 0, 0, 9, e.rd32(e.rd32(ctx) + 0xA8) + 0x14, 0, 0, 0);
    return 0;
}
}
