// 1.13c D2Game+0x65760 -- quest 0x25 tick: records the count left, notes the player, and when the level value is low enough and nothing is left closes the stage (flag 0xD, the three player steps, the timer); otherwise the warning step at five or fewer left
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f65760(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    if (!e.rd8(q + 9)) return NE::lo(eax0, 0);
    const uint32_t game = e.rd32(a), data = e.rd32(q + 0x18), g = e.rd32(game + 0x110);   // the vectors keep the block present: retail asserts on an absent one
    e.wr32(data + 0x88, e.rd32(g + 0x2cc) - e.rd32(g + 0x2d0));
    const uint32_t pl = e.rd32(a + 0xc);
    if (pl) {
        const uint32_t id = e.rd32(pl + 0xc);
        if (!e.h(0x33800, 0, 0, data, 0, id)) e.h(0x33880, data, 0, 0, 0, id);
    }
    const uint32_t idx = e.imp(0xA56E, 8) & 0xff;
    const uint32_t r = e.imp(0xB18C, e.rd32(game + idx * 4 + 0xbc), 8);
    const bool low = (int32_t)r <= (int32_t)e.rd32(g + 4);
    if (low && e.rd32(g + 0x2d0) == e.rd32(g + 0x2cc)) {
        if (e.trace) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFB2E0, 0, 0, e.rd32(q + 4), 4);
        e.wr8(data + 0x84, 1); e.wr32(q + 0xa8, 0);
        e.h(0x35EB0, 4, 0, 0, 0, q);
        const uint32_t fid = e.rd32(q + 0xe0);
        e.wr32(q + 0xc0, 0);
        e.h(0x33920, game, 0, 0, 0, 0, fid, 0xd);
        e.h(0x35A20, 0, 0, 0, e.rd32(q + 0xe0), 0, game, data, 0);
        e.cb(game, 0x65200, 0, e.rd32(a + 8));
        e.cb(game, 0x62CD0, 0, e.rd32(a + 8));
        e.cb(game, 0x62C60, 0, e.rd32(a + 8));
        const uint32_t g2 = e.rd32(q + 4);
        e.wr8(e.rd32(g2 + 0x10f4) + 0x20, 0);
        e.cb(g2, 0x359C0);
        if (e.rd8(data + 0x87)) return data;            // the vectors keep the mark clear: this exit answers the data pointer
        e.wr8(data + 0x87, 1);
        return e.h(0x33AF0, 0, 0, 0, 8, 0, q, 0xB2000000u | 0x63B50);
    }
    if (low && (int32_t)e.rd32(data + 0x88) <= 5) {
        e.wr8(q + 0x14, 0x20); e.wr8(q + 0xb, 4);
        const uint32_t v = e.cb(e.rd32(q + 4), 0x62D30);
        e.wr32(q + 0xa8, 0);
        return v;
    }
    if (e.rd8(q + 0xb) != 4 || (int32_t)e.rd32(data + 0x88) <= 5) return r;
    e.wr8(q + 0x14, 0x20); e.wr8(q + 0xb, 4);
    return e.cb(e.rd32(q + 4), 0x62D30);
}
}
