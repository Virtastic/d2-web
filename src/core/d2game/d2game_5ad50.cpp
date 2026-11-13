// 1.13c D2Game+0x5AD50 -- message handler: 0x6D advances the state 2 to 3, 0x79 raises the second message
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5ad50(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t eax = data;
    if (e.rd32(a + 0x14) == 0x6d && e.rd8(q + 0xc) == 2) { e.wr8(q + 0xc, 3); e.say(e.rd32(q + 4), 3); eax = e.cb(e.rd32(q + 4), 0x57C00); }
    if (e.rd32(a + 0x18) == 0x79 && e.rd8(q + 0xb) == 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); eax = e.cb(e.rd32(q + 4), 0x590C0); e.wr8(data + 0x86, 0); }
    return eax;
}
}
