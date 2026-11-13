// 1.13c D2Game+0xE8140 -- quest event handler: spawns class 0x20333367 for a live monster or a unit the check accepts, then marks the quest data and runs the callback
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe8140(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    if (!e.rd8(data + 2) || !e.rd8(q + 9)) return 0;
    const uint32_t npc = e.rd32(a + 8);
    bool ok = false;
    if (npc && e.rd32(npc) == 1) { const uint32_t ci = e.rd32(npc + 0x14); if (ci && (e.rd8(ci + 0x16) & 0xe)) ok = true; }
    if (!ok && !e.imp(0xA580, 0, npc)) return 0;
    const uint32_t game = e.rd32(q + 4);
    e.wr32(e.rd32(a + 8) + 0xb8, 0x20333367);
    uint32_t loc[4] = {};
    const uint32_t r = e.h(0x11A70, 0, 0, 0, game, 0, e.rd32(a + 8), 2, e.stk(loc), 0, ~0u, 0);
    if (!r) { e.wr8(data + 2, 0); return r; }
    e.wr8(data, 1);
    const uint32_t v = e.cb(game, 0xE46F0, 0, 0);
    e.wr32(q + 0xc0, 0);
    return v;
}
}
