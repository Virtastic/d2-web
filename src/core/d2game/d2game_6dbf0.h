// 1.13c D2Game+0x6DBF0 (object-operate monster drop set): eax = think context {+0 game, +4 unit, +0x10 monster id}, no result.
// A unit whose mode (+0x10) is nonzero is left alone. Otherwise: record = 0xA520(id), position = 0xA544(unit, &pos), then 0x6B570(ctx, 4) three times.
// 0xE390(eax = unit) must answer nonzero (else the fatal 0x594, not driven); unit+0xB8 = its answer and 0x11A70 runs twice (ebx = game; unit, 2, &slot, 0, -1, 1;
// slot = uninitialised frame residue, masked). The same again with 0xE310 (fatal 0x599). Then a record byte +0x140 of 0 sends mode 2 (0xA4A2); otherwise mode 1 and one 0x8EA80
// frame send at (record+0xDC >> 8) + 1 + game+0xA8. Last, bit 2 of unit+0xC4 is cleared.
#pragma once
#include <cstdint>

namespace r6dbf0 {
template <class E> void Run(E& e, uint32_t ctx)
{
    uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return;
    if (!unit) __builtin_trap();
    uint32_t slot = 0, pos[2] = {0, 0};   // slot: the 12-byte frame's lowest word, never written (stack residue, recorded masked)
    const uint32_t id = e.rd32(ctx + 0x10);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, id);
    e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(ctx + 4), e.stk(pos));
    for (int i = 0; i < 3; i++) e.h(0x6B570, 0, 0, 0, 0, 0, ctx, 4);
    for (int k = 0; k < 2; k++) {
        const uint32_t fn = k ? 0xE310 : 0xE390;
        const uint32_t v = e.h(fn, e.rd32(ctx + 4), 0, 0, 0, 0);
        if (!v) __builtin_trap();                       // retail: Fog 10265/10024 then _exit(-1)
        e.wr32(e.rd32(ctx + 4) + 0xB8, v);
        for (int j = 0; j < 2; j++) {
            e.h(0x11A70, 0, 0, 0, e.rd32(ctx), 0, e.rd32(ctx + 4), 2, e.stk(&slot), 0, 0xFFFFFFFFu, 1);
            e.blk(0x11A70, &slot, 1, 0u);
        }
    }
    unit = e.rd32(ctx + 4);
    if (e.rd8(rec + 0x140) == 0) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 2);
    } else {
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
        const uint32_t game = e.rd32(ctx);
        e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    }
    unit = e.rd32(ctx + 4);
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFFDu);
}
}
