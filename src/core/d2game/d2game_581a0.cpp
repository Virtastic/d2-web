// 1.13c D2Game+0x581A0 -- clears flag 4 and restores the quest state from the player flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f581a0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax;
    e.clrf(buf, fid, 4);
    eax = e.fl(buf, fid, 0); if (eax) return eax;
    eax = e.fl(buf, fid, 0xf); if (eax) return eax;
    eax = e.fl(buf, fid, 1); if (eax == 1) return eax;
    eax = e.fl(buf, fid, 3);
    if (eax == 1) { e.wr8(q + 0xc, 3); e.wr8(q + 0xb, 1); return eax; }
    eax = e.fl(buf, fid, 2);
    if (eax == 1) { e.wr8(q + 0xc, 2); e.wr8(q + 0xb, 1); }
    return eax;
}
}
