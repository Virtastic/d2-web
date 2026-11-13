// 1.13c D2Game+0xC2B00 -- npc greeting choice of quest 0xE: the six act npcs once after completion, then the progress flags and the state table
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc2b00(NE& e, uint32_t q, uint32_t a)
{
    static const uint32_t tab[8] = { 0xffffffffu, 0, 1, 2, 3, 4, 0x139, 0x138 };
    const uint32_t pl = e.rd32(a + 0xc), buf = e.qbuf(e.rd32(a), pl), npc = e.rd32(a + 8), m = e.rd32(a + 0x14);
    const uint32_t id = npc ? e.rd32(npc + 4) : 0xffffffffu;
    if (npc && id == 0xfb) {
        if (e.rd32(e.rd32(q + 0x18) + 0x18) != 2) return 2;
        return e.h(0x33EB0, 2, q, 0, 0, 0, m, 0xfb);
    }
    if (e.fl(buf, 0xe, 0xd)) {
        const uint32_t f = id == 0xb0 ? 6 : id == 0xaf ? 7 : id == 0xb1 ? 8 : id == 0xca ? 9 : id == 0xf4 ? 0xa : id == 0xb2 ? 0xb : 0;
        if (f && !e.fl(buf, 0xe, f)) return e.h(0x33EB0, 6, q, 0, 0, 0, m, id);
    }
    if (e.fl(buf, 0xe, 3)) return e.h(0x33EB0, 3, q, 0, 0, 0, m, id);
    if (e.fl(buf, 0xe, 4)) return e.h(0x33EB0, id == 0xd2 ? 5 : 4, q, 0, 0, 0, m, id);
    if (e.h(0x337D0, 0, 0, q, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu))) return e.h(0x33EB0, 4, q, 0, 0, 0, m, id);
    if (!e.rd8(q + 0xc)) return 0;
    const uint32_t x = e.fl(buf, 0xe, 0), y = e.fl(buf, 0xe, 0xd);
    if (x == 1 && !y) return 0;
    if (e.rd8(q + 0xc) >= 4 && !y) return 0;
    const uint32_t t = tab[e.rd8(q + 0xc) & 7];
    if (t == 1 && id == 0xb1) {
        if (e.h(0x338D0, e.rd32(q + 4), 0, 0, 0, 0, 0xc, 0xd) == 1) return 1;
    } else if (t == 0xffffffffu || t >= 8) return y;
    return e.h(0x33EB0, t, q, 0, 0, 0, m, id);
}
}
