// 1.13c D2Game+0x68D70 -- quest-5 monster callback. ECX = ctx {+0 game, +4 unit, +8 player, +0x10 class}. Written from the 1.13c disassembly.
// A unit with no mode (or none) gets 0xA4A2(unit, 1) and an event (0x8EA80, type 1, frame + class record +0xDC >> 8). Then the
// game's quest list (+0x10F4) is walked for type 5: active (+9) runs 0x34B80 (ECX unit, EAX player, 0x7F); state (+0xC) <= 1
// runs 0x35EB0 (EAX 2); flag +0xB clear sets data(+0x18)+0x11B. Always returns 0.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68d70(HookEnv& e, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    if (!unit || !e.rd32(unit + 0x10)) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
        const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
        const uint32_t game = e.rd32(ctx);
        e.h(0x8EA80, 0, game, e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
    }
    const uint32_t qs = e.rd32(e.rd32(ctx) + 0x10F4);
    if (!qs) return 0;
    uint32_t n = e.rd32(qs);
    while (n && e.rd32(n) != 5) n = e.rd32(n + 0xF4);
    if (!n || !e.rd8(n + 9)) return 0;
    e.h(0x34B80, e.rd32(ctx + 8), e.rd32(ctx + 4), 0, 0, 0, 0x7F);
    if (e.rd8(n + 0xC) > 1) return 0;
    e.h(0x35EB0, 2);
    if (e.rd8(n + 0xB) < 1) e.wr8(e.rd32(n + 0x18) + 0x11B, 1);
    return 0;
}
}
