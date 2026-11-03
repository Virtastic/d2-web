// 1.13c D2Game+0x6D740 (D2GAME_ObjectOperateOpen think entry): ecx = think context {+0 game, +4 unit, +0x10 monster id}, plain ret, returns 0.
// Rule: a non-null unit with +0x10 (mode) != 0 does nothing. Otherwise mode 1 (0xA4A2), clear bit 1 of unit+0xC4 when the unit is non-null,
// 0x6AD60 (no arguments), 0xA550 on the unit, then the same mode-1 frame send as 0x6D8F0 on the record of the id (0xA520, 0x8EA80).
#pragma once
#include <cstdint>

namespace r6d740 {
template <class E> uint32_t Open(E& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 0;
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    if (e.rd32(ctx + 4)) e.wr32(e.rd32(ctx + 4) + 0xC4, e.rd32(e.rd32(ctx + 4) + 0xC4) & ~2u);
    e.h(0x6AD60);
    e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    return 0;
}
}
