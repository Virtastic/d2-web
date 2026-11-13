// 1.13c D2Game+0x5A0A0 -- npc 0x201 activation: raises the pending quest messages
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5a0a0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), data = e.rd32(q + 0x18);
    uint32_t eax = npc ? e.rd32(npc + 4) : ~0u;
    if ((eax & 0xffff) != 0x201) return eax;
    eax = NE::lo(eax, e.rd8(data + 0xac));
    if (eax & 0xff) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); eax = e.cb(e.rd32(q + 4), 0x591D0); e.wr8(data + 0xac, 0); }
    eax = NE::lo(eax, e.rd8(q + 9)); if (!(eax & 0xff)) return eax;
    eax = NE::lo(eax, e.rd8(data + 0x101)); if (!(eax & 0xff)) return eax;
    eax = NE::lo(eax, 4);
    if (e.rd8(q + 0xb) >= 4) return eax;
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4);
    eax = e.cb(e.rd32(q + 4), 0x591D0);
    e.wr8(data + 0x101, 0);
    return eax;
}
}
