// 1.13c D2Game+0xE6FE0 -- ECX = quest record q with data d = q+0x18. The counter d+8 is decremented; when it reaches 0, q +9 and d +2 are nonzero and the q +0xC byte is at most 3: d +5 becomes 1, q +0x14 0 and +0xB 8, and the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xE6550, EDI = the +4 game, stack 0) and its result is returned. Otherwise d is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe6fe0(GE& e, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t n = e.rd32(d + 8) - 1;
    e.wr32(d + 8, n);
    if (n == 0 && e.rd8(q + 9) != 0 && e.rd8(d + 2) != 0 && e.rd8(q + 0xc) < 4) {
        e.wr8(d + 5, 1);
        e.wr8(q + 0x14, 0);
        e.wr8(q + 0xb, 8);
        const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe6550, 0, 0);
        e.j(0xDFCC0, e.rd32(q + 4));
        return v;
    }
    return d;
}
}
