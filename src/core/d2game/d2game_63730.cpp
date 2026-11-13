// 1.13c D2Game+0x63730 -- quest 3 step: marks the player flag 6 once, tags the player and moves the quest to step 2
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f63730(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t pl = e.rd32(a + 0xc), buf = e.qbuf(e.rd32(a), pl);
    if (!e.fl(buf, 3, 6)) {
        e.setf(buf, 3, 6);
        NE::wr16(pl + 0x6e, 0x24); e.wr32(pl + 0x70, pl);
        e.imp(0xA4DE, pl);
        e.wr32(pl + 0xc4, e.rd32(pl + 0xc4) | 0x400);
    }
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2);
    return e.cb(e.rd32(q + 4), 0x62AE0);
}
}
