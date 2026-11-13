// 1.13c D2Game+0xC72C0 -- Act 2 quest callback (ECX = quest node, EDX = event {+0 game, +0xC player unit}); written from the 1.13c disassembly.
// Reads the player's quest flag word (0xA4C0(player) + 0x10 + difficulty * 4). Flag (0xB, 0) or (0xB, 1) set: the game's quest holder (game +0x10F4,
// +8 must be set) gets flag (holder +0xC, 0xB, 0xD) through 0xA57A. Else flag (0xB, 2) set: 0xC7130(EAX node, EDI player), then node steps +0xB/+0xC
// become 2/3 with flag (0xB, 4), 2/2 with flag (0xB, 3), else 1/1. The result is the last flag test (or 0xA57A's).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc72c0(HookEnv& e, uint32_t node, uint32_t ev)
{
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xB, 0);
    if (r != 1) r = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xB, 1);
    if (r == 1) {
        const uint32_t hd = e.rd32(e.rd32(node + 4) + 0x10F4);
        return e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xC), 0xB, 0xD);
    }
    r = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xB, 2);
    if (r != 1) return r;
    e.h(0xC7130, node);
    e.edi(0xC7130, pl);
    r = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xB, 4);
    if (r) { e.wr8(node + 0xB, 2); e.wr8(node + 0xC, 3); return r; }
    r = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xB, 3);
    if (r) { e.wr8(node + 0xB, 2); e.wr8(node + 0xC, 2); return r; }
    e.wr8(node + 0xB, 1);
    e.wr8(node + 0xC, 1);
    return r;
}
}
