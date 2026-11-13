// 1.13c D2Game+0xB9320 -- restores the quest 0x19 state from the player flags when a player joins
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb9320(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t r = e.fl(buf, fid, 0);
    if (r) return r;
    r = e.fl(buf, fid, 0xf);
    if (r) return r;
    r = e.fl(buf, 0x19, 4);
    if (r == 1) { e.wr8(e.rd32(q + 0x18) + 0xc, 1); e.wr8(q + 0xb, 2); e.wr8(q + 0xc, 3); return r; }
    r = e.fl(buf, 0x19, 3);
    if (r == 1) { e.wr8(q + 0xc, 3); e.wr8(q + 0xb, 1); return r; }
    r = e.fl(buf, 0x19, 2);
    if (r == 1) { e.wr8(q + 0xc, 2); e.wr8(q + 0xb, 1); }
    return r;
}
}
