// 1.13c D2Game+0x5A720 -- npc 0x1FF activation: raises the pending quest messages
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5a720(NE& e, uint32_t q, uint32_t a)
{
    uint32_t eax = e.rd32(a + 8);
    if (!eax || NE::rd16(eax + 4) != 0x1ff) return eax;
    const uint32_t data = e.rd32(q + 0x18);
    eax = NE::lo(eax, 1);
    if (e.rd8(data + 8) == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0x14, 0); eax = e.cb(e.rd32(q + 4), 0x59620); e.wr8(data + 8, 0); eax = NE::lo(eax, 1); }
    if (e.rd8(data + 0x16) == 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4); eax = e.cb(e.rd32(q + 4), 0x59620); e.wr8(data + 0x16, 0); }
    return eax;
}
}
