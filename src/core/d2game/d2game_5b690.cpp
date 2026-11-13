// 1.13c D2Game+0x5B690 -- first entry of the quest 0x26 area: arms the per-player callbacks, state 4 and the timed callback
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5b690(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    if (!e.rd8(q + 9)) return NE::lo(eax0, 0);
    uint32_t eax = e.imp(0xA478, e.rd32(a + 8));
    if (!eax) return eax;
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    e.wr32(data + 0x8c, eax);
    e.cb(game, 0x57900);
    e.cb(e.rd32(a), 0x5AF50, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0x59060, 0, e.rd32(a + 8));
    e.cb(e.rd32(a), 0x58FF0, 0, e.rd32(a + 8));
    e.h(0x33920, game, 0, 0, 0, 0, 0x26, 0xd);
    e.wr8(e.rd32(game + 0x10f4) + 0x20, 0x11);
    eax = e.cb(game, 0x359C0);
    e.wr8(q + 0xc, 4);
    e.say(game, 4);
    if (e.rd8(data + 0x85)) return data;
    e.wr8(data + 0x85, 1);
    return e.h(0x33AF0, 0, 0, 0, 8, 0, q, 0xB2000000u | 0x59CD0);
}
}
