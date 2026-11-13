// 1.13c D2Game+0xE9A10 -- quest boss-killed state callback: state 6 bookkeeping, the four player callbacks, the quest unit release and the 0xb list state
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe9a10(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(q + 4), data = e.rd32(q + 0x18);
    e.wr8(q + 0xc, 6);
    e.say(game, 6);
    e.wr32(q + 0xc8, 0xB2000000u | 0xE3390);
    if (e.rd8(q + 9)) {
        const uint32_t pl = e.rd32(a + 0xc);
        if (pl) {
            const uint32_t idx = e.rd8(game + 0x6d);
            uint32_t buf = e.rd32(e.imp(0xA4C0, pl) + 0x10 + 4 * idx);
            if (!e.fl(buf, 0x16, 0)) {
                buf = e.rd32(e.imp(0xA4C0, pl) + 0x10 + 4 * idx);
                if (!e.fl(buf, 0x16, 0xb)) e.wr32(data + 0x1c, e.rd32(data + 0x1c) + 1);
                buf = e.rd32(e.imp(0xA4C0, pl) + 0x10 + 4 * idx);
                e.h(0xE34D0, buf, pl, 0, 0, 0, e.rd32(a));
            }
        }
        static const uint32_t F[4] = {0xE5C40, 0xE6850, 0xE5B10, 0xE3320};
        for (const uint32_t f : F) e.cb(e.rd32(a), f, 0, e.rd32(a + 8));
        if (!e.rd8(data)) {
            e.h(0x33AF0, 0, 0, 0, 0xC, 0, q, 0xB2000000u | 0xE68B0);
            e.wr8(data, 1);
        }
    }
    const uint32_t list = e.rd32(e.rd32(a) + 0x10f4);
    if (!e.rd32(list + 8)) return 0;
    e.setf(e.rd32(list + 0xc), 0x16, 0xd);
    if (e.rd8(data + 1)) {
        const uint32_t gm = e.rd32(q + 4), id = e.rd32(data + 4);
        if (!gm) return 0;
        for (uint32_t n = e.rd32(gm + 0x1520 + (id & 0x7f) * 4); n; n = e.rd32(n + 0xe4)) {
            if (e.rd32(n + 0xc) != id) continue;
            e.imp(0xA4A2, n, 1);
            const uint32_t o = e.imp(0xA520, e.rd32(n + 4));
            e.h(0x8EA80, 0, gm, n, 0, 0, 1, (e.rd32(o + 0xdc) >> 8) + e.rd32(gm + 0xa8), 0, 0, 0);
            break;
        }
    }
    e.wr32(data + 0xc, 2);
    e.wr32(e.rd32(a + 8) + 0xb8, 0x2073736d);
    for (int i = 0; i < (int)e.rd32(data + 0x1c); ++i) {
        uint32_t loc[4] = {};
        if (e.h(0x11A70, 0, 0, 0, e.rd32(a), 0, e.rd32(a + 8), 2, e.stk(loc), 0, ~0u, 0)) {
            e.wr8(data + 0x18, 1);
            e.wr32(data + 0x14, e.rd32(data + 0x14) + 1);
        }
    }
    const uint32_t g2 = e.rd32(q + 4);
    e.wr8(e.rd32(g2 + 0x10f4) + 0x20, 0xb);
    return e.cb(g2, 0x359C0, 0, 0);
}
}
