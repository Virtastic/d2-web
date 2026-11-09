// 1.13c D2Game+0xEAA30: EAX = game, ESI = monster. The monster's class (+4; -1 for null) 0x2C0 / 0x2C1 / 0x2C5 sets game+0x1DF0 / +0x1DEC / +0x1DE8
// (any other class is fatal, line 0xE6). When all three are set: monster+0xB8 = 'cm2 '; item = 0x11A70(monster, 7, &local, 0, -1, 0); 0xD2380(EAX = game,
// ECX = 0, stack item); monster+0xB8 = 'std '; then game+0x8C times 0x11A70(monster, 2, &local, 0, -1, 0).
#pragma once
#include <cstdint>
namespace geaa30 {
template <class E, class Fatal> void Run(E& e, Fatal& fatal, uint32_t game, uint32_t mon)
{
    // 1.13c D2Game+0xEAA30
    uint32_t local = 0;
    const uint32_t k = mon ? e.rd32(mon + 4) : 0xFFFFFFFFu;
    if (k == 0x2C0) e.wr32(game + 0x1DF0, 1);
    else if (k == 0x2C1) e.wr32(game + 0x1DEC, 1);
    else if (k == 0x2C5) e.wr32(game + 0x1DE8, 1);
    else { fatal(0xE6u); return; }
    if (!e.rd32(game + 0x1DF0) || !e.rd32(game + 0x1DEC) || !e.rd32(game + 0x1DE8)) return;
    const uint32_t lp = e.stk(&local);
    e.wr32(mon + 0xB8, 0x20326D63);
    const uint32_t a[6] = { mon, 7, lp, 0, 0xFFFFFFFFu, 0 };
    const uint32_t item = e.h(0x11A70, 0, 0, 0, 0, 0, a[0], a[1], a[2], a[3]); e.blk(0x11A70, a, 6);
    e.h(0xD2380, game, 0, 0, 0, 0, item);
    e.wr32(mon + 0xB8, 0x20647473);
    const uint32_t b[6] = { mon, 2, lp, 0, 0xFFFFFFFFu, 0 };
    for (int32_t n = (int32_t)e.rd32(game + 0x8C); n > 0; --n) { e.h(0x11A70, 0, 0, 0, 0, 0, b[0], b[1], b[2], b[3]); e.blk(0x11A70, b, 6); }
}
}
