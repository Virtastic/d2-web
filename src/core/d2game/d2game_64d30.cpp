// 1.13c D2Game+0x64D30 -- join message of the quest 3: first sight of an open quest moves it to step 1 and saves the flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f64d30(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m14 = e.rd32(a + 0x14), game = e.rd32(q + 4);
    if (!e.rd8(q + 9)) { e.wr32(q + 0xac, 0); return m14; }
    if (m14 != 1 || e.rd8(q + 0xc) != 2) return m14;
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax = e.fl(buf, fid, 0); if (eax == 1) return eax;
    eax = e.fl(buf, fid, 1); if (eax == 1) return eax;
    if (e.rd8(q + 0xb) != 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); e.cb(game, 0x62AE0); }
    e.h(0x35EB0, 3);
    eax = e.cb(game, 0x61DA0);
    e.wr32(q + 0xac, 0);
    return eax;
}
}
