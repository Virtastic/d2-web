// 1.13c D2Game+0xE8010 -- quest event handler: when armed and the npc is reachable it spawns class 0x2034336a, then moves to state 1 and runs the callback
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe8010(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    if (!e.rd8(q + 9) || !e.rd8(data + 2) || !e.rd8(data + 0xc)) return 0;
    const uint32_t game = e.rd32(q + 4), pl = e.rd32(a + 0xc);
    if (pl) { const uint32_t r = e.fl(e.qbuf(game, pl), e.rd32(q + 0xe0), 0); if (r) return r; }
    e.wr32(e.rd32(a + 8) + 0xb8, 0x2034336a);
    uint32_t loc[4] = {};
    uint32_t eax = e.h(0x11A70, 0, 0, 0, game, 0, e.rd32(a + 8), 2, e.stk(loc), 0, ~0u, 0);
    if (!eax) { e.wr8(data + 1, 1); e.wr8(data + 2, 0); return eax; }
    e.wr32(q + 0xc0, 0);
    e.wr32(data + 0x10, e.rd32(data + 0x10) + 1);
    e.wr8(data + 0xc, 0); e.wr8(data + 0x14, 1);
    if (e.rd8(q + 0xc) < 1) eax = e.h(0x35EB0, 1);
    eax = NE::lo(eax, e.rd8(q + 0xb));
    if (e.rd8(q + 0xb)) { e.wr8(data + 2, 0); return eax; }
    e.wr8(q + 0xb, 1);
    eax = e.cb(game, 0xE5F00, 0, 0);
    e.wr8(data + 2, 0);
    return eax;
}
}
