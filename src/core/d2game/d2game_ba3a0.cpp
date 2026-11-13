// 1.13c D2Game+0xBA3A0 -- messages 0x67 and 0x6C of quest 0x1A: finish the quest for a player without it, and open it when the portal message arrives
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fba3a0(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    uint32_t eax = eax0;
    const uint32_t game = e.rd32(q + 4);
    if (e.rd32(a + 0x14) == 0x67 && e.rd8(q + 0xc) == 2) {
        eax = e.fl(e.qbuf(e.rd32(a), e.rd32(a + 0xc)), e.rd32(q + 0xe0), 0);
        if (eax == 1) return eax;
        if (e.rd8(q + 0xc) < 3) e.h(0x35EB0, 3);
        if (e.rd8(q + 0xb) < 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); }
        eax = e.cb(game, 0xB9290);
    }
    if (e.rd32(a + 0x18) != 0x6c) return eax;
    eax = NE::lo(eax, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return eax;
    eax = 3;
    if (e.rd8(q + 0xc) < 3) eax = e.h(0x35EB0, 3);
    eax = NE::lo(eax, 1);
    if (e.rd8(q + 0xb) < 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0x14, 0); e.cb(game, 0xB99D0); }
    return e.cb(game, 0xB9290);
}
}
