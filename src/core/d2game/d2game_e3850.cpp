// 1.13c D2Game+0xE3850 -- Act 3 quest state restore from the player's flags (ECX = quest node, EDX = event {+0 game, +0xC player unit}); written from the 1.13c disassembly.
// Flags are the player's word (0xA4C0(player) + 0x10 + difficulty * 4); 0xA568(flags, group, n) reads one. Group 0x12 flag 0 set: node data (+0x18) byte +0xC = 1.
// Group 0x15 flag 0 or 4 set: group 0x15 flag 0xD is set through 0xA57A on the game's quest holder (game +0x10F4, +8 must be set; its absence ends retail) and node byte +9
// clears (the result is the 0xA57A return). Else node data +8 = group 0x11 flag 0 and group 0x15 flags 2 / 3 pick node step (+0xB) and state (+0xC): only 2 gives step 2,
// state 2 (data +8 set) or 3; 3 (with or without 2) gives step 3, state 4 (data +8 set) or 5; neither leaves them. The result is data +8 on a step change, else the last flag read.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fe3850(HookEnv& e, uint32_t node, uint32_t ev)
{
    const uint32_t game = e.rd32(ev);
    const uint32_t d = e.rd32(node + 0x18);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xC));
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x12, 0)) e.wr8(d + 0xC, 1);
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x15, 0) == 1 || e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x15, 4) == 1) {
        const uint32_t hd = e.rd32(e.rd32(node + 4) + 0x10F4);
        const uint32_t r = e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xC), 0x15, 0xD);
        e.wr8(node + 9, 0);
        return r;
    }
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x11, 0);
    e.wr32(d + 8, v);
    const uint32_t f2 = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x15, 2);
    const uint32_t f3 = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x15, 3);
    if (!f2 && !f3) return f3;
    if (f3) { e.wr8(node + 0xB, 3); e.wr8(node + 0xC, v ? 4 : 5); return v; }
    e.wr8(node + 0xB, 2); e.wr8(node + 0xC, v ? 2 : 3); return v;
}
}
