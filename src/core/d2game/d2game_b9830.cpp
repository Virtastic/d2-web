// 1.13c D2Game+0xB9830 -- restores the quest 0x1B state from the player flags and inventory when a player joins
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb9830(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), pl = e.rd32(a + 0xc), buf = e.qbuf(game, pl), data = e.rd32(q + 0x18), fid = e.rd32(q + 0xe0);
    if (e.h(0xE660, pl, 0, 0, game, 0, 0x20686668)) e.wr32(data + 0x10, e.rd32(data + 0x10) + 1);
    uint32_t r = e.fl(buf, fid, 0);
    if (r) return r;
    r = e.fl(buf, fid, 0xf);
    if (r) return r;
    r = e.h(0xE660, pl, 0, 0, game, 0, 0x2073736d);
    if (!r) { e.wr8(q + 0xc, 1); return 0; }
    r = e.fl(buf, 0x1b, 5);
    if (r == 1) { e.wr8(data + 0x1c, 1); e.wr8(q + 0xb, 4); e.wr8(q + 0xc, 2); return r; }
    r = e.fl(buf, 0x1b, 3);
    if (r == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 3); return r; }
    r = e.fl(buf, 0x1b, 2);
    if (r == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 2); }
    return r;
}
}
