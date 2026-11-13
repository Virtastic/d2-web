// 1.13c D2Game+0xC2680 -- npc 0x17A greeting: with quest 0xD open and neither flag of quest 0xE the npc gets its quest greeting
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc2680(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : 0xffffffffu, game = e.rd32(q + 4);
    const uint32_t pd = e.imp(0xA4C0, e.rd32(a + 0xc)), buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (id != 0x17a) return pd;
    const uint32_t n = e.quest(game, 0xd);
    if (!n || !e.rd8(n + 9) || e.rd8(n + 0xc) >= 2) return game;
    uint32_t r = e.h(0xC21A0, game);
    if ((r & 0xff) != 1) return r;
    r = e.fl(buf, 0xe, 1);
    if (r) return r;
    r = e.fl(buf, 0xe, 0);
    if (r) return r;
    return e.h(0x33EB0, 0, q, 0, 0, 0, e.rd32(a + 0x14), 0x17a);
}
}
