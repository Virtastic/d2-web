// 1.13c D2Game+0xBC2C0 -- quest 0x1A npc event: three deaths of the others arm the timer once all five marks are set; npc 0xF3 records its room, starts the clock and runs the five player steps
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fbc2c0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), npc = e.rd32(a + 8);
    if (!npc || e.rd32(npc + 4) != 0xf3) {
        const uint32_t c = e.rd32(data + 0x48) + 1;
        e.wr32(data + 0x48, c);
        if (c != 3) return c;
        for (uint32_t k = 0; k < 5; k++) if (!e.rd8(data + 0xc + k)) return k;
        const uint32_t r = e.h(0xBB3B0, data, e.rd32(q + 4));
        if (e.rd8(data + 1)) return NE::lo(r, e.rd8(data + 1));
        e.wr32(data + 0x18, 0); e.wr8(data + 2, 1); e.wr8(data + 3, 0); e.wr8(data + 1, 1);
        return e.h(0x33AF0, 0, 0, 0, 1, 0, q, 0xB2000000u | 0xBBDC0);
    }
    const uint32_t game = e.rd32(q + 4);
    if (!e.rd32(game + 0x70)) {
        e.wr8(e.rd32(game + 0x10f4) + 0x20, 0xd);
        e.cb(game, 0x359C0);
    }
    e.wr8(data + 0x14, 1);
    uint32_t r = e.imp(0xA478, e.rd32(a + 8));
    if (!r) return 0;
    e.wr32(data + 0x3c, r);
    r = e.rd32(a + 0xc);
    if (r) {
        const uint32_t d = e.rd32(q + 0x18);
        r = d;                                          // the vectors skip an inactive record whose clock already runs: that exit answers the data pointer
        if (!e.rd8(d + 1)) {
            e.wr8(d + 2, 0); e.wr8(d + 3, 1); e.wr8(d + 4, 1); e.wr8(d + 5, 1); e.wr8(d + 0x15, 0); e.wr8(d + 1, 1);
            e.wr32(d + 0x18, e.h(0xF8D10));             // the import GetTickCount through its slot
            r = e.h(0x33AF0, 0, 0, 0, 1, 0, q, 0xB2000000u | 0xBBDC0);
        }
        if (!e.rd8(q + 9)) return NE::lo(r, 0);
        r = e.fl(e.qbuf(e.rd32(a), e.rd32(a + 0xc)), 0x1a, 0);
    }
    if (!e.rd8(q + 9)) return NE::lo(r, 0);
    e.wr32(data + 0x1c, 0); e.wr8(q + 0xb, 0xd);
    e.cb(e.rd32(q + 4), 0xB99D0);
    e.cb(e.rd32(a), 0xB9F20, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0xBA490, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0xB9BC0, 0, e.rd32(a + 8));
    return e.cb(e.rd32(a), 0xB9B00, 0, e.rd32(a + 8));
}
}
