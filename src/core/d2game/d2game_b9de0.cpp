// 1.13c D2Game+0xB9DE0 -- npc 0xF6 talk end: runs the pending quest step and saves the quest to the players
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb9de0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8);
    if (!npc) return 0;
    if (NE::rd16(npc + 4) != 0xf6) return npc;
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    if (e.rd8(data + 2) != 1) return NE::lo(npc, 1);
    e.wr8(q + 0xb, e.rd8(data + 0x1c) ? 4 : 1);
    e.cb(game, 0xB9970);
    e.wr8(q + 0x14, 0); e.wr8(data + 2, 0);
    e.wr32(q + 0xa8, 0);
    return e.cb(game, 0xB8E80);
}
}
