// 1.13c D2Game+0xBB1B0 -- npc 0x16F talk end: runs the pending quest step, then the expansion follow-up while its two bytes allow
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fbb1b0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8);
    if (!npc) return 0;
    if (NE::rd16(npc + 4) != 0x16f) return npc;
    const uint32_t data = e.rd32(q + 0x18);
    if (e.rd8(data) == 1) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
        e.cb(e.rd32(q + 4), 0xB99D0);
        e.wr8(data, 0);
    }
    uint32_t eax = e.rd32(e.rd32(q + 4) + 0x70);
    if (!eax) return eax;
    eax = NE::lo(eax, e.rd8(data + 0x44));
    if (!e.rd8(data + 0x44)) return eax;
    eax = NE::lo(eax, e.rd8(data + 0x45));
    if (e.rd8(data + 0x45)) return eax;
    return e.h(0xBAFF0, a, 0, 0, 0, 0, data);
}
}
