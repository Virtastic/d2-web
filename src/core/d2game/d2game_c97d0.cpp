// 1.13c D2Game+0xC97D0 -- Act 2 "killed the quest monster for the quest" callback (ECX = quest node, EDX = event {+0 game, +8 unit id, +0xC player unit}); written from the 1.13c disassembly.
// With the global dbg set 0xCBE70 logs the entry. A node with +9 set takes state (+0xC) 3 (debug log around it), runs 0x33AF0(EBX 8; node, C58B0) and, with a player in the event, sets
// the player's flag (group 0xE, 5) when flags 0, 3, 4 and 5 of that group are all clear, then walks the callbacks of the unit class 0xDFA60 names (0x9C160(EAX class, ESI game; C29E0)
// unless the class is 0xFFFF). Then in every case: node +0xA8 and +0xC0 clear, node data (+0x18) byte +1 = 1 and +0x60 = 0xA478(unit id), the game's quest holder (game +0x10F4)
// byte +0x20 = 8 and the callbacks 359C0 and C5820 are registered through 0xDFCC0 (EDI = game). A data byte +5 then looks up the unit under data +0x2C in the game's table at
// +0x1520 (hash &0x7F, chain +0xE4): a hit runs 0xA4A2(unit, 1) and 0x8EA80(ECX game, EDX unit; 1, 0xA520(unit +4)[+0xDC] >> 8 + game +0xA8, 0, 0, 0). Data +0x18 becomes 2.
// The result is the last callee's EAX (the game pointer when the lookup misses). A null game (exit in retail) is not modelled.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc97d0(HookEnv& e, uint32_t node, uint32_t ev, uint32_t dbg)
{
    uint32_t eax = 0;
    if (dbg) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFB240, 0, 0, e.rd32(node + 4), 4);
    if (e.rd8(node + 9)) {
        e.wr8(node + 0xC, 3);
        if (dbg) { char buf[128]; e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, e.rd32(node + 4), 4); }
        e.h(0x33AF0, 0, 0, 0, 8, 0, node, 0xB2000000u | 0xC58B0);
        const uint32_t pl = e.rd32(ev + 0xC);
        if (pl) {
            const uint32_t game = e.rd32(ev);
            const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
            const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
            if (!e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xE, 0) && !e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xE, 3) &&
                !e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xE, 4) && !e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xE, 5)) {
                e.h(0xA57A, 0, 0, 0, 0, 0, fl, 0xE, 5);
                const uint32_t cls = e.h(0xDFA60, pl);
                if ((uint16_t)cls != 0xFFFF) e.h(0x9C160, cls, 0, 0, 0, game, 0xB2000000u | 0xC29E0);
            }
        }
    }
    const uint32_t d = e.rd32(node + 0x18);
    e.wr32(node + 0xA8, 0);
    e.wr32(node + 0xC0, 0);
    e.wr8(d + 1, 1);
    e.wr32(d + 0x60, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ev + 8)));
    const uint32_t g = e.rd32(node + 4);
    e.wr8(e.rd32(g + 0x10F4) + 0x20, 8);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x359C0, 0, 0); e.edi(0xDFCC0, g);
    eax = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC5820, 0, 0); e.edi(0xDFCC0, e.rd32(node + 4));
    eax &= ~0xFFu;
    if (e.rd8(d + 5)) {
        const uint32_t key = e.rd32(d + 0x2C), game = e.rd32(node + 4);
        eax = game;
        uint32_t u = e.rd32(game + 0x1520 + (key & 0x7F) * 4);
        while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
        if (u) {
            e.h(0xA4A2, 0, 0, 0, 0, 0, u, 1);
            const uint32_t b = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(u + 4));
            eax = e.h(0x8EA80, 0, game, u, 0, 0, 1, (e.rd32(b + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
            { const uint32_t z = 0; e.x(0x8EA80, &z, 1); }
        }
    }
    e.wr32(d + 0x18, 2);
    return eax;
}
}
