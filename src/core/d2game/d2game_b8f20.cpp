// 1.13c D2Game+0xB8F20 -- restores the quest 0x1A state from the player flags when a player joins
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb8f20(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    if (e.rd32(game + 0x70) && e.fl(buf, 0x1a, 9) && !e.fl(buf, 0x1c, 0)) e.clrf(buf, 0x1a, 9);
    uint32_t r = e.fl(buf, fid, 0);
    if (r) return r;
    r = e.fl(buf, fid, 0xf);
    if (r) return r;
    r = e.fl(buf, 0x1a, 4);
    if (r == 1) { e.wr8(q + 0xb, 2); e.wr8(q + 0xc, 3); return r; }
    r = e.fl(buf, 0x1a, 3);
    if (r == 1) { e.wr8(q + 0xc, 3); e.wr8(q + 0xb, 1); return r; }
    r = e.fl(buf, 0x1a, 2);
    if (r == 1) { e.wr8(q + 0xc, 2); e.wr8(q + 0xb, 1); }
    return r;
}
}
