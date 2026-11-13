// 1.13c D2Game+0xE7DB0 -- quest event handler: counts the npc class 0x20316671 and 0x20786f62 spawns, and when the timer runs out opens the next state
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7db0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), data = e.rd32(q + 0x18);
    uint32_t eax = npc;
    if (npc && e.rd32(npc + 4) == 0x16e) { const uint32_t v = e.rd32(npc + 0xc4) | 0x20000; e.wr32(npc + 0xc4, v); return npc; }
    const uint32_t game = e.rd32(q + 4);
    const auto spawn = [&](uint32_t kind) { uint32_t loc[4] = {}; return e.h(0x11A70, 0, 0, 0, game, 0, e.rd32(a + 8), kind, e.stk(loc), 0, ~0u, 0); };
    if (!e.rd8(data + 0xd) || !e.rd8(data + 0xe)) {
        e.wr32(data + 0x3c, 0); e.wr32(data + 0x40, 0);
        eax = e.cb(game, 0xE36A0, 0, 0);
    }
    eax = NE::lo(eax, e.rd8(data + 0xd));
    if (!e.rd8(data + 0xd)) {
        e.wr32(e.rd32(a + 8) + 0xb8, 0x20316671);
        const int32_t n = (int32_t)e.rd32(data + 0x3c);
        eax = n; uint32_t cnt = 0;
        if (n > 0) {
            for (int32_t i = 0; i < n; i++) { if (spawn(7)) cnt++; eax = e.rd32(data + 0x3c); }
            eax = cnt;
            if (cnt) { e.wr8(data + 0xd, 1); eax = e.h(0xE2DB0, game, 0, 0, 0, 0, cnt); }
        }
    } else {
        eax = NE::lo(eax, e.rd8(data + 0xe));
        if (!e.rd8(data + 0xe)) {
            e.wr8(data + 0xe, 1);
            e.wr32(e.rd32(a + 8) + 0xb8, 0x20786f62);
            eax = e.rd32(data + 0x40);
            for (int32_t i = 0; (int32_t)eax > i; ) { spawn(2); eax = e.rd32(data + 0x40); i++; }
        }
    }
    eax = NE::lo(eax, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return eax;
    eax = e.rd32(data + 0x34);
    if ((int32_t)eax <= 0) return eax;
    eax--; e.wr32(data + 0x34, eax);
    if (eax) return eax;
    const uint32_t n2 = e.rd32(a + 8);
    e.wr32(data, n2 ? e.rd32(n2 + 0xc) : ~0u);
    e.wr8(q + 0x14, 0);
    eax = NE::lo(eax, e.rd8(data + 0xc));
    if (e.rd8(data + 0xc)) { eax = e.h(0x35EB0, 7); e.wr8(q + 0xb, 0xd); }
    else { eax = e.h(0x35EB0, 6); e.wr8(q + 0xb, 4); }
    const uint32_t g = e.rd32(a);
    e.cb(game, 0xE5D20, 0, 0);
    e.cb(game, 0xE5D80, 0, 0);
    e.cb(g, 0xE6980, 0, e.rd32(a + 8));
    e.cb(g, 0xE5CD0, 0, e.rd32(a + 8));
    e.cb(g, 0xE3760, 0, e.rd32(a + 8));
    return e.h(0x33920, game, 0, 0, 0, 0, 0x15, 0xd);
}
}
