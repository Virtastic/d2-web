// 1.13c D2Game+0x65A70 -- npc reward spawn: marks the player flag once, runs the player step and drops eight items
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f65a70(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t pl = e.rd32(a + 0xc), game = e.rd32(a);
    if (pl) {
        uint32_t r = e.fl(e.qbuf(game, pl), 4, 0xa);
        if (r) return r;
        r = e.fl(e.qbuf(game, pl), e.rd32(game + 0x70) ? 0x28 : 0x1a, 0);
        if (!r) return 0;
        e.setf(e.qbuf(game, pl), 4, 0xa);
    }
    const uint32_t npc = e.rd32(a + 8);
    e.cb(game, 0x61500, 0, npc);
    e.wr32(npc + 0xb8, 0x20737076);
    uint32_t local = q, r = 0;
    for (int k = 0; k < 8; k++) r = e.h(0x11A70, 0, 0, 0, game, 0, npc, 0, e.stk(&local), 0, 0xffffffffu, 0);
    return r;
}
}
