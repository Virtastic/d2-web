// 1.13c D2Game+0xE3560 -- Act 3 quest flag restore (ECX = game, EDX = player unit, one unused stack word); written from the 1.13c disassembly.
// Reads the player's flag word (0xA4C0(player) + 0x10 + difficulty * 4, difficulty byte game +0x6D). Group 0x16 flag 0 or 0xB set: nothing. Else the game's list holder
// (game +0x10F4, head = its first word, next at +0xF4) is searched for the first kind 0x14 entry and its state (+0xC) / step (+0xB) pick the flag set through
// 0xA57A(flags, 0x16, n): state 2 sets 2, state 3 sets 3, state 4 sets 4 / 5 / 6 for step 2 / 3 / 4, state 5 sets 7 / 8 / 9 for step 2 / 3 / 4. The result is always 0.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fe3560(HookEnv& e, uint32_t game, uint32_t pl)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x16, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, 0x16, 0xB) == 1) return 0;
    const uint32_t hd = e.rd32(game + 0x10F4);
    if (!hd) return 0;
    uint32_t s = e.rd32(hd);
    if (!s) return 0;
    while (e.rd32(s) != 0x14) { s = e.rd32(s + 0xF4); if (!s) return 0; }
    const uint32_t st = e.rd8(s + 0xC), sub = e.rd8(s + 0xB);
    uint32_t n = 0;
    if (st == 2) n = 2;
    else if (st == 3) n = 3;
    else if (st == 4 && sub >= 2 && sub <= 4) n = 2 + sub;
    else if (st == 5 && sub >= 2 && sub <= 4) n = 5 + sub;
    if (n) e.h(0xA57A, 0, 0, 0, 0, 0, fl, 0x16, n);
    return 0;
}
}
