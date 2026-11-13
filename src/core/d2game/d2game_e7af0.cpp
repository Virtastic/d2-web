// 1.13c D2Game+0xE7AF0 -- quest event handler: when the quest is armed it moves to state 2 with the state callback and then asks for the player id
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7af0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t eax = NE::lo(data, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return eax;
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2);
    e.cb(e.rd32(q + 4), 0xE6550, 0, 0);
    e.wr8(q + 0xc, 4);
    e.say(e.rd32(q + 4), 4);
    const uint32_t u = e.rd32(a + 0xc);
    return e.h(0x33880, data + 0x1c, 0, 0, 0, u ? e.rd32(u + 0xc) : ~0u);
}
}
