// 1.13c D2Game+0x5D520 -- npc 0x200 activation: raises the pending quest message and hands the reward
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5d520(NE& e, uint32_t q, uint32_t a)
{
    uint32_t eax = e.rd32(a + 8);
    if (!eax || NE::rd16(eax + 4) != 0x200) return eax;
    const uint32_t data = e.rd32(q + 0x18);
    eax = NE::lo(eax, 1);
    if (e.rd8(data + 0x86) == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0x14, 0); eax = e.cb(e.rd32(q + 4), 0x590C0); e.wr8(data + 0x86, 0); }
    eax = NE::lo(eax, e.rd8(data + 0x87)); if (!(eax & 0xff)) return eax;
    eax = NE::lo(eax, e.rd8(data + 0x88)); if (eax & 0xff) return eax;
    return e.h(0x5D380, e.rd32(a + 8), 0, 0, 0, 0, q);
}
}
