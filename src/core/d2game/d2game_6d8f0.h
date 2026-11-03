// 1.13c D2Game+0x6D8F0 (AITHINK_ResetToStandMode): ecx = think context {+0 game, +4 unit, +0x10 monster id}, plain ret, returns 1.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call.
// A null unit, or one whose +0x10 (mode) is 0, runs the body: mode 1 (0xA4A2), clear bit 1 of unit+0xC4 when the unit is non-null,
// the monster record of the id (0xA520), a mode-1 send at (record+0xDC >> 8) + 1 + game+0xA8 (0x8EA80, eax free, ecx = game, edx = unit,
// stack = 1, frame, 0, 0, 0), then 0xA550 on the unit. Any other unit: nothing.
#pragma once
#include <cstdint>

namespace r6d8f0 {
template <class E> uint32_t Reset(E& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 1;
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    if (e.rd32(ctx + 4)) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & ~2u);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    return 1;
}
}
