// 1.13c D2Game+0xE3120 -- Act 3 quest callback picking the node state from the player's quest flags (ECX = quest node, EDX = event {+0 game, +0xC player unit}); written from the 1.13c disassembly.
// Reads the player's flag word (0xA4C0(player) + 0x10 + difficulty * 4), then tests flags in group 0x16 in order 0, 0xB, (group 0x17, 0): any set sends 0xA57A(holder +0xC, 0x16, 0xD)
// (the holder is game +0x10F4, +8 must be set) and sets the node data (+0x18) dword +0xC to 2. Else the first set of group 0x16 flags 9, 8, 7, 6, 5, 4, 3, 2 stores node
// state (+0xC) / step (+0xB) of 5/4, 5/3, 5/2, 4/4, 4/3, 4/2, 2/2, 3/2. The result is the last callee's EAX.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fe3120(HookEnv& e, uint32_t node, uint32_t ev)
{
    const uint32_t game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xC));
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    auto t = [&](uint32_t g, uint32_t n) { return e.h(0xA568, 0, 0, 0, 0, 0, fl, g, n); };
    uint32_t r = t(0x16, 0);
    if (r == 1 || (r = t(0x16, 0xB)) == 1 || (r = t(0x17, 0)) == 1) {
        const uint32_t hd = e.rd32(e.rd32(node + 4) + 0x10F4);
        r = e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xC), 0x16, 0xD);
        e.wr32(e.rd32(node + 0x18) + 0xC, 2);
        return r;
    }
    static const uint8_t tb[8][3] = { {9, 5, 4}, {8, 5, 3}, {7, 5, 2}, {6, 4, 4}, {5, 4, 3}, {4, 4, 2}, {3, 2, 2}, {2, 3, 2} };
    for (int i = 0; i < 8; ++i) {
        r = t(0x16, tb[i][0]);
        if (r == 1) { e.wr8(node + 0xC, tb[i][1]); e.wr8(node + 0xB, tb[i][2]); return r; }
    }
    return r;
}
}
