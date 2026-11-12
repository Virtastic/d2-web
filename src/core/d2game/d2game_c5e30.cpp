// 1.13c D2Game+0xC5E30 -- EAX = whatever the caller left (its upper three bytes come back when no call is made), ECX unused, EDX = quest record. With the record's +0xC byte equal to 0x4: its +0x14 byte becomes 0, its +0xB byte 0x3 and the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xC52B0, EDI = the record's +4 game, stack 0). The +0x6 byte of the record's +0x18 data is cleared on both paths; AL = 1 over the upper bytes of the worker's result (or of EAX at entry).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc5e30(GE& e, uint32_t a, uint32_t, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    uint32_t v = a;
    if (e.rd8(q + 0xc) == 0x4) {
        e.wr8(q + 0x14, 0);
        e.wr8(q + 0xb, 0x3);
        v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xc52b0, 0, 0);
        e.j(0xDFCC0, e.rd32(q + 4));
    }
    e.wr8(d + 0x6, 0);
    return (v & ~0xffu) | 1;
}
}
