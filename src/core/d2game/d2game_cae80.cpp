// 1.13c D2Game+0xCAE80 -- per-client packet-loop entry. EAX = pointer to the client id, stack s0 (ret 4); `tick` is GetTickCount.
// Written from the 1.13c disassembly: lock (0xA364), game (0x2AAE0), walk game+0x88 by +0x4A8 for the id
// (absent: unlock 0x29BA0), client = 0x49FE0(game, id) (null: fatal 499 via 0xA2C2/0xA2DA), client+0x3D8 = tick, and 0xCC550(game, s0)
// when 0x4AD10(0) answers a record whose first dword is 0. Unlock.
#include "d2game_seam4e.h"
extern "C" void d2game_cae80(uint32_t idp, uint32_t s0, uint32_t tick, const D2Seam* e)
{
    const uint32_t id = d2rd32(idp);
    const uint32_t lk = e->call(0xA364, 0, 0, 0, 0, 0, id);
    const uint32_t game = e->call(0x2AAE0, 0, 0, 0, 0, 0, lk);
    if (!game) return;
    uint32_t c = d2rd32(game + 0x88);
    if (c) {
        while (d2rd32(c) != id) {
            c = d2rd32(c + 0x4A8);
            if (!c) { e->call(0x29BA0, game); return; }
        }
        const uint32_t cl = e->call(0x49FE0, game, id);
        if (!cl) {
            const uint32_t m = e->call(0xA2C2, 0, 0, 0, 0, 0, 499);
            e->call(0xA2DA, 0, 0, 0, 0, 0, 0, m);
            return;
        }
        d2wr32(cl + 0x3D8, tick);
        const uint32_t rec = e->call(0x4AD10, 0, 0, 0, 0, 0, 0);
        if (rec && d2rd32(rec) == 0) e->call(0xCC550, 0, 0, 0, 0, 0, game, s0);
    }
    e->call(0x29BA0, game);
}
