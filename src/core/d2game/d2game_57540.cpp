// 1.13c D2Game+0x57540 -- restores the quest state from the player flags when a player joins
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f57540(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax;
    eax = e.fl(buf, fid, 0);
    if (!eax) eax = e.fl(buf, fid, 0xf);
    if (eax) { eax = e.rd32(q + 0x18); e.wr8(eax + 0x4a, 1); return eax; }
    eax = e.fl(buf, fid, 3);
    if (eax == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 3); return eax; }
    eax = e.fl(buf, fid, 2);
    if (eax == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 2); }
    return eax;
}
}
