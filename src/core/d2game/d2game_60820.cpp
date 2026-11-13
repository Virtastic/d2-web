// 1.13c D2Game+0x60820 -- npc availability test of the quest 1 for the npc 0x10A
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f60820(NE& e, uint32_t eax0, uint32_t q, uint32_t id, uint32_t unit, uint32_t s1, uint32_t s2)
{
    if (id != 0x10a) return NE::lo(eax0, 0);
    const uint32_t game = e.rd32(q + 4);
    const uint32_t n = e.quest(game, 1);
    if (!e.rd8(n + 9)) return NE::lo(game, 0);
    uint32_t eax = e.h(0x338D0, game, 0, 0, 0, 0, 1, 0xd);
    if (eax) return NE::lo(eax, 0);
    const uint32_t buf = e.qbuf(game, unit);
    eax = e.fl(buf, 1, 0); if (eax) return NE::lo(eax, 0);
    eax = e.fl(buf, 1, 0xd); if (eax) return NE::lo(eax, 0);
    eax = e.fl(buf, 1, 1); if (eax) return NE::lo(eax, 0);
    return NE::lo(eax, 1);
}
}
