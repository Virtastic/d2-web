// 1.13c D2Game+0x635D0 -- npc 0x94 talk end: runs the two pending quest 4 steps; npc 0x92 sets its seen byte
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f635d0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : 0xffffffffu, data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax = id;
    if ((id & 0xffff) == 0x94) {
        eax = NE::lo(eax, e.rd8(data + 0x53));
        if (e.rd8(data + 0x53) == 1) {
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
            e.cb(game, 0x62A80, 0, e.rd32(a + 0xc));
            e.wr8(data + 0x53, 0);
            eax = e.cb(game, 0x619B0);
        }
        if (e.rd8(data + 0x64) == 1) {
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3);
            e.cb(game, 0x62A80, 0, e.rd32(a + 0xc));
            eax = e.cb(game, 0x619B0);
            e.wr8(data + 0x64, 0);
        }
        return eax;
    }
    if ((id & 0xffff) == 0x92) e.wr8(data + 0x93, 1);
    return eax;
}
}
