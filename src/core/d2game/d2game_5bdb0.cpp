// 1.13c D2Game+0x5BDB0 -- first entry of the quest 0x23 area: arms the per-player callbacks and raises the first message
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5bdb0(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    if (!e.rd8(q + 9)) return NE::lo(eax0, 0);
    uint32_t eax = e.imp(0xA478, e.rd32(a + 8));
    if (!eax) return eax;
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    e.wr32(data + 4, eax);
    e.cb(game, 0x586C0);
    e.cb(e.rd32(a), 0x5B490, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0x595C0, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0x59550, 0, e.rd32(a + 8));
    e.h(0x33920, game, 0, 0, 0, 0, 0x23, 0xd);
    e.wr8(e.rd32(game + 0x10f4) + 0x20, 0xf);
    eax = e.cb(game, 0x359C0);
    eax = NE::lo(eax, 3);
    if (e.rd8(q + 0xb) >= 3) return eax;
    e.wr8(q + 0xb, 3); e.wr8(q + 0x14, 0);
    return e.cb(game, 0x59620);
}
}
