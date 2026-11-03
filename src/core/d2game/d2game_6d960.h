// 1.13c D2Game+0x6D960 (AITHINK_LegWoundStandDown): ecx = think context {+0 game, +4 unit, +0x10 monster id}, plain ret, returns 1.
// A unit whose +0x10 (mode) is nonzero is left alone. Otherwise unit+0xB8 = 0x2067656C ('leg ') and 0x11A70 is called (ebx = game; stack = unit, 2,
// &slot holding the ctx pointer, 0, -1, 0). When it answers nonzero: 0xA4A2(unit, 1), the monster record of the id (0xA520), then two 0x8EA80 sends
// (ecx = game, edx = unit): type 1 at (record+0xDC >> 8) + 1 + game+0xA8, type 7 at game+0xA8 + 10 (stack = type, frame, 0, 0, 0).
// A null unit would write through address 0xB8 in retail (fault): not driven.
#pragma once
#include <cstdint>

namespace r6d960 {
template <class E> uint32_t Run(E& e, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 1;
    e.wr32(e.rd32(ctx + 4) + 0xB8, 0x2067656C);
    uint32_t slot = ctx;
    const uint32_t ok = e.h(0x11A70, 0, 0, 0, e.rd32(ctx), 0, e.rd32(ctx + 4), 2, e.stk(&slot), 0, 0xFFFFFFFFu, 0);
    e.blk(0x11A70, &slot, 1);
    if (!ok) return 1;
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 1);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    uint32_t game = e.rd32(ctx);
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    game = e.rd32(ctx);
    e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 7, e.rd32(game + 0xA8) + 10, 0, 0, 0);
    return 1;
}
}
