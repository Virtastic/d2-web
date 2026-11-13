// 1.13c D2Game+0x63870 -- npc 0x9A talk end: runs the pending quest 3 step
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f63870(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8);
    if (!npc) return 0;
    if (NE::rd16(npc + 4) != 0x9a) return npc;
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax;
    if (e.rd8(data + 2) == 1) {
        e.cb(game, 0x61DA0);
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
        eax = e.cb(game, 0x62AE0);
        e.wr8(data + 2, 0);
        return eax;
    }
    if (e.rd8(data + 3) == 1) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        eax = e.cb(game, 0x62AE0);
        e.wr8(data + 3, 0);
        return eax;
    }
    return NE::lo(npc, 1);
}
}
