// 1.13c D2Game+0x6D7B0 (AITHINK_IdleSpawnRoll): ecx = think context {+0 game, +4 unit, +8 owner, +0xC seed pointer, +0x10 monster id}, plain ret, returns 0.
// Rule: a non-null unit with +0x10 (mode) != 0 does nothing. Otherwise 0xA556(0xA478(unit)); an owner whose +0 is 0 asks 0xA3D0(owner, 1, -1) for a handle h and,
// when h is non-zero, runs 0xACD0(owner, unit), r = 0xAE1A(h) and 0x78430(game; h, r, 2, unit id or -1, 0; edi = owner). Then mode 1 (0xA4A2), clear unit+0xC4 bit 1,
// 0xA550. Roll % 10000 with bits above 0x1FFF set: 0xB510(ctx, 8) and 0x6A050(ebx = that result, esi = ctx, 8). Roll % 100 <= 20: 0x6B570(ctx, 0).
// Last, the record of the monster id (0xA520) and a mode-1 frame send at (record+0xDC >> 8) + 1 + frame (0x8EA80).
#pragma once
#include <cstdint>
#include "d2game_6ce10.h"

namespace r6d7b0 {
template <class E> uint32_t IdleSpawnRoll(E& e, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 0;
    e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit));
    const uint32_t owner = e.rd32(ctx + 8);
    if (owner && e.rd32(owner) == 0) {
        const uint32_t h = e.h(0xA3D0, 0, owner, 1, 0, 0, 0xFFFFFFFFu);
        if (h) {
            e.h(0xACD0, 0, 0, 0, 0, 0, owner, e.rd32(ctx + 4));
            const uint32_t u = e.rd32(ctx + 4);
            const uint32_t r = e.h(0xAE1A, 0, 0, 0, 0, 0, h);
            e.h(0x78430, e.rd32(ctx), 0, 0, 0, 0, h, r, 2, u ? e.rd32(u + 0xC) : 0xFFFFFFFFu, 0);
        }
    }
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 1);
    const uint32_t u2 = e.rd32(ctx + 4);
    if (u2) e.wr32(u2 + 0xC4, e.rd32(u2 + 0xC4) & ~2u);
    e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    if (r6ce10::Roll(e, e.rd32(ctx + 0xC), 10000) & 0xFFFFE000u) {
        const uint32_t b = e.h(0xB510, ctx, 0, 0, 0, 0, 8);
        e.h(0x6A050, 0, 0, 0, b, ctx, 8);
    }
    if (r6ce10::Roll(e, e.rd32(ctx + 0xC), 100) <= 0x14) e.h(0x6B570, 0, 0, 0, 0, 0, ctx, 0);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    const uint32_t game = e.rd32(ctx);
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    return 0;
}
}
