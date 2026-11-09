// 1.13c D2Game+0x6DEC0 -- AI think: timed idle tick. ECX = ctx {+0 game, +4 unit, +0x10 class}; returns 1. `now` is
// GetTickCount (the FN descriptor pins it). Written from the 1.13c disassembly.
// room = 0xA478(unit), 0xA544(unit, &c), 0xA71E(unit), 0xB13E(unit) (results unused). Before unit+0xD4 + 500 nothing more.
// Then rec = 0xA520(class). Mode 0: 0xA550(unit), 0xA4A2(unit, 1), +0xD4 = now, event 1 at game+0xA8 + (rec+0xDC >> 8) + 1
// through 0x8EA80(ECX game, EDX unit; 1, frame, 0, 0, 0). Mode 2: 0xA69A(unit, room, c.x, c.y), 0xA4A2(unit, 0), +0xD4 = now.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f6dec0(HookEnv& e, uint32_t ctx, uint32_t now)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    uint32_t c[2] = { 0, 0 };
    e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(ctx + 4), e.stk(c));
    e.h(0xA71E, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    e.h(0xB13E, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    const uint32_t unit = e.rd32(ctx + 4);
    const uint32_t mode = unit ? e.rd32(unit + 0x10) : 0;
    if (now < e.rd32(unit + 0xD4) + 0x1F4) return 1;
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    if (mode == 0) {
        e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
        e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 1);
        e.wr32(e.rd32(ctx + 4) + 0xD4, now);
        const uint32_t game = e.rd32(ctx);
        e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(game + 0xA8) + 1, 0, 0, 0);
    } else if (mode == 2) {
        e.h(0xA69A, 0, 0, 0, 0, 0, e.rd32(ctx + 4), room, c[0], c[1]);
        e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(ctx + 4), 0);
        e.wr32(e.rd32(ctx + 4) + 0xD4, now);
    }
    return 1;
}
}
