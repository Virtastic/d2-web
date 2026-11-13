// 1.13c D2Game+0xC6E90 -- Act 2 quest callback (ECX = quest node, EDX = event {+0 game, +8, +0xC player unit, +0x14 and +0x18 event words});
// written from the 1.13c disassembly. Only event word 0xB0 acts; the player's quest flag word is 0xA4C0(player) + 0x10 + difficulty * 4.
// Word 0x130: node data +8 = 1, state (+0xC) = 2, optional debug log (global dbg), registers callback C4430 through 0xDFCC0, then 0x35910.
// Word 0x14E: needs flag (node +0xE0, 1); with flag 0xD clear it runs the callback in the linked unit's node (0x33F30) when the data byte +0x10 is
// set; with it set and state != 5 it moves to step 0xD (callback C5630, 0x35EB0(5), node callback +0xF0) and with +9 clear sends 0x33920.
// Both finish with flags (E0, 0) cleared and (E0, 1) set, the party relay 0x33880 and 0x35910. Any other word 0xB0 argument leaves stale stack bits in EAX (not modelled).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc6e90(HookEnv& e, uint32_t node, uint32_t ev, uint32_t dbg)
{
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t w14 = e.rd32(ev + 0x14) & 0xFFFF, w18 = e.rd32(ev + 0x18) & 0xFFFF;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (w14 != 0xB0) return pd;
    const uint32_t d = e.rd32(node + 0x18), g = e.rd32(node + 4), k = e.rd32(node + 0xE0);
    if (w18 == 0x130) {
        e.wr8(d + 8, 1);
        e.wr8(node + 0xC, 2);
        char buf[128];
        if (dbg) e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, g, 4);
        e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC4430, 0, 0);
        e.edi(0xDFCC0, g);
        return e.h(0x35910, e.rd32(ev + 8), pl, 0, 0, 0, game);
    }
    if (w18 != 0x14E) return 0xDEAD0000u | w18;
    uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, fl, k, 1);
    if (!r) return r;
    if (!e.h(0xA568, 0, 0, 0, 0, 0, fl, k, 0xD)) {
        if (e.rd8(d + 0x10)) {
            const uint32_t x = e.h(0x33F30, g, e.rd32(node + 0x10));
            if (x) e.h(e.rd32(x + 0xF0) & 0xFFFFFF, 0, x);
        }
    } else {
        if (e.rd8(node + 0xC) != 5) {
            e.wr8(node + 0x14, 0);
            e.wr8(node + 0xB, 0xD);
            e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC5630, 0, 0);
            e.edi(0xDFCC0, g);
            e.wr32(node + 0xA8, 0);
            e.h(0x35EB0, 5, 0, 0, 0, node);
            e.h(e.rd32(node + 0xF0) & 0xFFFFFF, 0, node);
        }
        if (!e.rd8(node + 9)) e.h(0x33920, g, 0, 0, 0, 0, 9, 0xD);
    }
    e.h(0xA57A, 0, 0, 0, 0, 0, fl, k, 0);
    e.h(0xA586, 0, 0, 0, 0, 0, fl, k, 1);
    e.h(0x33880, node + 0x1C, 0, 0, 0, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu);
    return e.h(0x35910, e.rd32(ev + 8), pl, 0, 0, 0, game);
}
}
