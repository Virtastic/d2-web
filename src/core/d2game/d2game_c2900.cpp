// 1.13c D2Game+0xC2900 -- restores the quest 0xE state from the player flags when a player joins
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc2900(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0), data = e.rd32(q + 0x18);
    e.clrf(buf, 0xa, 9);
    if (e.fl(buf, 0xa, 0)) { e.wr8(data + 0xc, 1); e.wr8(data + 0xd, 1); }
    if (e.fl(buf, fid, 0) || e.fl(buf, fid, 0xf)) { e.wr8(e.rd32(q + 0x18) + 0x3d, 1); return e.rd32(q + 0x18); }
    uint32_t r = e.fl(buf, 0xe, 2);
    if (r == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 2); return r; }
    r = e.fl(buf, 0xe, 3);
    if (r == 1) { e.wr8(q + 0xb, 5); e.wr8(q + 0xc, 5); return 5; }
    r = e.fl(buf, 0xe, 4);
    if (r == 1) { e.wr8(q + 0xb, 6); e.wr8(q + 0xc, 5); }
    return r;
}
}
