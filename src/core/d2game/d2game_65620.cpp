// 1.13c D2Game+0x65620 -- quest 6 start: state 4, records the npc, runs the four player steps and arms the timer
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f65620(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    if (!e.rd8(q + 9)) return NE::lo(eax0, 0);
    e.wr8(q + 0xc, 4);
    e.say(e.rd32(q + 4), 4);
    const uint32_t data = e.rd32(q + 0x18), npc = e.rd32(a + 8), game = e.rd32(a);
    e.wr8(data + 1, 1); e.wr8(data + 2, 1);
    e.wr32(data + 8, (npc ? e.rd32(npc + 0xc) : 0xffffffffu));
    e.cb(game, 0x63930, 0, npc);
    e.cb(game, 0x64FE0, 0, npc);
    e.cb(game, 0x62BB0, 0, npc);
    e.cb(game, 0x62B40, 0, npc);
    e.h(0x33AF0, 0, 0, 0, 0xf, 0, q, 0xB2000000u | 0x63A80);
    e.wr32(q + 0xa8, 0);
    e.wr8(data, 1); e.wr32(data + 4, 1);
    const uint32_t holder = e.rd32(game + 0x10f4);
    return e.setf(e.rd32(holder + 0xc), e.rd32(q + 0xe0), 0xd);
}
}
