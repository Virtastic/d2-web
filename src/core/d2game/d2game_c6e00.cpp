// 1.13c D2Game+0xC6E00 -- Act 2 quest callback (ECX = quest node, EDX = event {+0 game, +0xC player unit, +0x14 event id}); written from the 1.13c disassembly.
// Only event 0x28 acts. A node with a nonzero word at +0x9C first runs the party broadcast 0x33830 (ECX node + 0x1C, EAX player id or -1).
// A node in state 2 then reads the player's quest flags (0xA4C0(player) + 0x10 + difficulty * 4); with flag (node +0xE0, 0) or (node +0xE0, 1)
// set it stops, else it runs 0x35EB0(EAX 3) on the node and registers the callback C4430 through 0xDFCC0 (EDI = game). The result is the last callee's EAX.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc6e00(HookEnv& e, uint32_t eax, uint32_t node, uint32_t ev)
{
    if (e.rd32(ev + 0x14) != 0x28) return eax;
    if ((uint16_t)e.rd16s(node + 0x9C)) {
        const uint32_t pl = e.rd32(ev + 0xC);
        eax = e.h(0x33830, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu, node + 0x1C);
    }
    if (e.rd8(node + 0xC) != 2) return eax;
    const uint32_t game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xC));
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, e.rd32(node + 0xE0), 0) == 1) return 1;
    if (e.h(0xA568, 0, 0, 0, 0, 0, fl, e.rd32(node + 0xE0), 1) == 1) return 1;
    e.h(0x35EB0, 3, 0, 0, 0, node);
    const uint32_t g = e.rd32(node + 4);
    const uint32_t r = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC4430, 0, 0);
    e.edi(0xDFCC0, g);
    return r;
}
}
