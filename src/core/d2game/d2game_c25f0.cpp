// 1.13c D2Game+0xC25F0 -- npc 0x17A menu test: quest 0xD open, the helper agrees and the player has neither flag of quest 0xE
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc25f0(NE& e, uint32_t eax0, uint32_t q, uint32_t id, uint32_t unit, uint32_t s1, uint32_t s2)
{
    if (id != 0x17a) return NE::lo(eax0, 0);
    const uint32_t game = e.rd32(q + 4), buf = e.qbuf(game, unit), n = e.quest(game, 0xd);
    if (!n || !e.rd8(n + 9) || e.rd8(n + 0xc) >= 2) return NE::lo(game, 0);
    uint32_t r = e.h(0x338D0, game, 0, 0, 0, 0, 0xb, 0xd);
    if (!r) return 0;
    r = e.fl(buf, 0xe, 1);
    if (r) return NE::lo(r, 0);
    r = e.fl(buf, 0xe, 0);
    return r ? NE::lo(r, 0) : 1;
}
}
