// 1.13c D2Game+0xC3080 -- npc greeting choice of quest 0xD: flag 1, the list of served players, then the state table
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc3080(NE& e, uint32_t q, uint32_t a)
{
    static const uint32_t tab[8] = { 0xffffffffu, 0, 1, 2, 0xc9, 0x1ae, 1, 0 };
    const uint32_t pl = e.rd32(a + 0xc), buf = e.qbuf(e.rd32(a), pl), npc = e.rd32(a + 8), m = e.rd32(a + 0x14);
    const uint32_t id = npc ? e.rd32(npc + 4) : 0xffffffffu;
    if (e.fl(buf, 0xd, 1)) return e.h(0x33EB0, 1, q, 0, 0, 0, m, id);
    const uint32_t uid = (pl ? e.rd32(pl + 0xc) : 0xffffffffu), cnt = NE::rd16(q + 0x9c);
    uint32_t k = 0;
    for (; k < cnt; k++) if (e.rd32(q + 0x1c + 4 * k) == uid) return e.h(0x33EB0, 2, q, 0, 0, 0, m, id);
    if (!e.rd8(q + 9) || !e.rd8(q + 0xc)) return NE::lo(k, 0);
    const uint32_t x = e.fl(buf, 0xd, 0), y = e.fl(buf, 0xd, 0xd);
    if (x == 1 && !y) return 0;
    if (e.rd8(q + 0xc) >= 2 && !y) return 0;
    const uint32_t t = tab[e.rd8(q + 0xc) & 7];
    if (t == 0xffffffffu || t >= 4) return t;
    return e.h(0x33EB0, t, q, 0, 0, 0, m, id);
}
}
