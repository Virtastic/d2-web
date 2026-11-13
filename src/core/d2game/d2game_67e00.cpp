// 1.13c D2Game+0x67E00 -- quest start: the first player gets three gem drops from the npc, three player steps run, the timer is armed and the state becomes 4
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f67e00(NE& e, uint32_t q, uint32_t a)
{
    static const uint32_t gemA[7] = { 0x20766367, 0x20726367, 0x20626367, 0x20796367, 0x20676367, 0x20776367, 0x20636b73 };
    static const uint32_t gemB[7] = { 0x20767367, 0x20727367, 0x20627367, 0x20797367, 0x20677367, 0x20777367, 0x20756b73 };
    const uint32_t data = e.rd32(q + 0x18);
    e.wr32(q + 0xa8, 0);
    if (e.rd8(q + 9)) {
        const uint32_t pl = e.rd32(a + 0xc), game = e.rd32(q + 4);
        if (pl && !e.fl(e.qbuf(game, pl), 6, 0) && !e.fl(e.qbuf(game, pl), 6, 1)) {
            e.h(0x62640, e.qbuf(game, pl), pl, 0, 0, 0, game);
            uint32_t local = 0;
            for (int k = 0; k < 3; k++) {
                const uint32_t hold = e.rd32(e.rd32(q + 4) + 0x10f4);       // retail asserts a present holder
                const uint64_t s = (uint64_t)e.rd32(hold + 0x18) * 0x6ac690c5u + e.rd32(hold + 0x1c);
                e.wr32(hold + 0x1c, (uint32_t)(s >> 32)); e.wr32(hold + 0x18, (uint32_t)s);
                const uint32_t npc = e.rd32(a + 8);
                e.wr32(npc + 0xb8, (k < 2 ? gemA : gemB)[(uint32_t)s % 7]);
                e.h(0x11A70, 0, 0, 0, e.rd32(q + 4), 0, npc, 2, e.stk(&local), 0, 0xffffffffu, 0);
            }
            e.wr32(e.rd32(a + 8) + 0xb8, 0);
        }
        e.cb(e.rd32(a), 0x630D0, 0, e.rd32(a + 8));
        e.cb(e.rd32(a), 0x63D70, 0, e.rd32(a + 8));
        e.cb(e.rd32(a), 0x625F0, 0, e.rd32(a + 8));
    }
    const uint32_t npc = e.rd32(a + 8);
    e.wr32(data + 0x18c, (npc ? e.rd32(npc + 0xc) : 0xffffffffu));
    NE::wr16(data + 0x192, 1);
    if (e.rd8(q + 9)) {
        e.h(0x33AF0, 0, 0, 0, 1, 0, q, 0xB2000000u | 0x67C60);
        e.cb(e.rd32(q + 4), 0x626C0);
    }
    e.wr8(data + 0x194, 1); e.wr8(q + 0xc, 4);
    e.say(e.rd32(q + 4), 4);
    e.wr32(q + 0xc8, 0xB2000000u | 0x60A30);
    e.wr32(q + 0xc0, 0);
    return 0;
}
}
