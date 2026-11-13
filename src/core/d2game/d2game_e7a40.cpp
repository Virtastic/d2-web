// 1.13c D2Game+0xE7A40 -- quest event handler: the talk step resolves the player and, when the quest is armed, moves to state 3 and runs the state callback
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7a40(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t u = e.rd32(a + 0xc);
    uint32_t eax = e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, e.rd32(q + 0x18) + 0x1c);
    eax = NE::lo(eax, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return eax;
    e.wr8(q + 0xc, 3);
    e.say(e.rd32(q + 4), 3);
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
    return e.cb(e.rd32(q + 4), 0xE6550, 0, 0);
}
}
