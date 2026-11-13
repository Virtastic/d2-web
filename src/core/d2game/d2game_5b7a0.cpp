// 1.13c D2Game+0x5B7A0 -- message handler of the npc 0x200: 0x4EA9 opens the reward, 0x4EB4 closes the talk
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5b7a0(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax = (eax0 & 0xffff0000u) | NE::rd16(a + 0x18);
    if (NE::rd16(a + 0x14) != 0x200) return eax;
    if ((eax & 0xffff) == 0x4ea9) {
        eax = NE::lo(eax, e.rd8(q + 9)); if (!(eax & 0xff)) return eax;
        e.h(0x35EB0, 2);
        e.wr8(data + 0x86, 1); e.wr8(data + 0x87, 1);
        return e.cb(game, 0x57C00);
    }
    if ((eax & 0xffff) != 0x4eb4) return eax;
    e.wr8(q + 0x14, 0);
    e.cb(game, 0x590C0);
    e.setf(e.qbuf(game, e.rd32(a + 0xc)), 0x26, 4);
    eax = e.fl(e.qbuf(game, e.rd32(a + 0xc)), 0x26, 0xd);
    if (!eax) {
        eax = NE::lo(eax, e.rd8(q + 9)); if (eax & 0xff) return eax;
        eax = e.h(0x1020, e.rd32(a + 0xc), 0, 0, 0, 0, game);
        if (!(eax = e.fl(eax, 0x26, 1))) return eax;
    }
    e.h(0x35EB0, 5);
    return e.h(0x5AFB0, 0, q);   // tail call
}
}
