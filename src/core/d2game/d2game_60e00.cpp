// 1.13c D2Game+0x60E00 -- restores the quest 5 state from the player flags 4, 6, 5, 3, 2
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f60e00(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax;
    eax = e.fl(buf, fid, 0); if (eax) return eax;
    eax = e.fl(buf, fid, 0xf); if (eax) return eax;
    eax = e.fl(buf, 5, 4);
    if (eax != 1) {
        eax = e.fl(buf, 5, 6);
        if (eax == 1) { e.wr8(q + 0xb, 4); e.wr8(q + 0xc, 3); return eax; }
        eax = e.fl(buf, 5, 5);
        if (eax == 1) { e.wr8(q + 0xb, 3); e.wr8(q + 0xc, 2); return eax; }
        eax = e.fl(buf, 5, 3);
        if (eax != 1) {
            eax = e.fl(buf, 5, 2);
            if (eax == 1) { e.wr8(q + 0xc, 2); e.wr8(q + 0xb, 1); }
            return eax;
        }
    }
    e.wr8(q + 0xc, 3); e.wr8(q + 0xb, 1);
    return eax;
}
}
