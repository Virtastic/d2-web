// 1.13c D2Game+0x68FC0 -- ECX = ctx {+0 game, +4 unit, +8 player, +0x10 class}; returns 0. Written from the 1.13c disassembly.
// The type-4 quest (game +0x10F4 list) must be active (+9), its data (+0x18) not done (+0x50 != 1) and its state < 6. The
// player's quest flags (0xA4C0(player) + 0x10 + difficulty*4) with bit (4,1) or (4,0) set send 0xDFFD0(0x13, player). A unit
// still in a mode stops; otherwise it gets mode 1 and event 1 (frame + class rec+0xDC >> 8), the quest data +0x54 = 3 and
// +0x3C = player id (-1 none), event 7 at frame + 0x11, 0xA51A(0xA478(unit), 0), flags (4,0xD) and (4,1) via 0xA57A,
// 0x34C00(EAX player; game), and a player whose 0xDFA60 is not 0xFFFF gets 0x9C160(ESI game, EBX 0; 0x629B0).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68fc0(HookEnv& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx);
    uint32_t q = e.rd32(game + 0x10F4);
    if (q) { q = e.rd32(q); while (q && e.rd32(q) != 4) q = e.rd32(q + 0xF4); }
    uint32_t qd = 0;
    if (q) {
        qd = e.rd32(q + 0x18);
        if (!e.rd8(q + 9) || e.rd8(qd + 0x50) == 1 || e.rd8(q + 0xC) >= 6) return 0;
    }
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ctx + 8));
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, 4, 1) || e.h(0xA568, 0, 0, 0, 0, 0, fl, 4, 0)) {
        e.h(0xDFFD0, 0, 0, 0, 0, 0, 0x13, e.rd32(ctx + 8));
        return 0;
    }
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) return 0;
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    e.h(0x8EA80, 0, e.rd32(ctx), e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(e.rd32(ctx) + 0xA8), 0, 0, 0);
    if (qd) {
        e.wr32(qd + 0x54, 3);
        const uint32_t pl = e.rd32(ctx + 8);
        e.wr32(qd + 0x3C, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu);
    }
    e.h(0x8EA80, 0, e.rd32(ctx), e.rd32(ctx + 4), 0, 0, 7, e.rd32(e.rd32(ctx) + 0xA8) + 0x11, 0, 0, 0);
    const uint32_t st = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    e.h(0xA51A, 0, 0, 0, 0, 0, st, 0);
    e.h(0xA57A, 0, 0, 0, 0, 0, fl, 4, 0xD);
    e.h(0xA57A, 0, 0, 0, 0, 0, fl, 4, 1);
    e.h(0x34C00, e.rd32(ctx + 8), 0, 0, 0, 0, e.rd32(ctx));
    if ((e.h(0xDFA60, e.rd32(ctx + 8)) & 0xFFFF) != 0xFFFF)
        e.h(0x9C160, 0, 0, 0, 0, e.rd32(ctx), 0xB2000000u | 0x629B0);
    return 0;
}
}
