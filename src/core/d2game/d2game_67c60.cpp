// 1.13c D2Game+0x67C60 -- quest timer tick: the tenth tick runs the step on the stored unit, the twelfth moves the quest to phase 3
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f67c60(NE& e, uint32_t game, uint32_t q)
{
    const uint32_t data = e.rd32(q + 0x18), w = (NE::rd16(data + 0x192) + 1) & 0xffff;
    NE::wr16(data + 0x192, w);
    if (w == 0xa) {
        const uint32_t id = e.rd32(data + 0x18c);
        uint32_t u = e.rd32(game + 0x1320 + 4 * (id & 0x7f));
        while (u && e.rd32(u + 0xc) != id) u = e.rd32(u + 0xe4);
        return NE::lo(e.cb(game, 0x67320, 0, u), 0);
    }
    if (w != 0xc) return NE::lo(data, 0);
    const uint32_t b = e.rd8(q + 0xb);
    if (b == 3 || b == 0xd) return NE::lo(data, 1);
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3);
    return NE::lo(e.cb(e.rd32(q + 4), 0x62700), 1);
}
}
