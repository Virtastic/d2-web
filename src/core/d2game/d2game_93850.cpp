// 1.13c D2Game+0x93850 -- state event on a live unit: builds the nine-word request from the record and hands it over, then applies the five stat pairs to the answer and, on a monster that passes the test, refreshes it
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f93850(NE& e, uint32_t p, uint32_t u)
{
    const uint32_t s = e.rd32(p + 0xc);
    if (!(u && !(e.rd8(u + 0xc6) & 1) && ((e.rd32(u) == 0 && e.rd32(u + 0x10) != 0 && e.rd32(u + 0x10) != 0x11) || (e.rd32(u) == 1 && e.rd32(u + 0x10) != 0 && e.rd32(u + 0x10) != 0xc)))) return 0;
    const int32_t len = (int32_t)e.rd32(s + 0x10);
    uint32_t b[9] = { e.rd32(p + 4), u, e.rd32(s), e.rd32(s + 4), e.rd32(s + 8), len > 0 ? (uint32_t)len : 0xffffffffu, e.rd32(s + 0x28), e.rd32(s + 0xc), 0xB2000000u | 0x50130 };
    const uint32_t r = e.h(0xA00B0, e.stk(b));
    e.blk(0xA00B0, b, 9);
    if (!r) return 0;
    for (uint32_t k = 0; k < 5; k++) {
        const uint32_t v = e.rd32(s + 0x14 + k * 4);
        if ((int32_t)v >= 0) e.imp(0xA6E2, r, v, e.rd32(s + 0x2c + k * 4), 0);
    }
    if (e.rd32(u) == 1 && e.h(0x9F120, u, 0, 0, 0, 0, 0xa)) {
        const uint32_t d = e.rd32(u + 0x14);
        e.h(0x9D230, 0, e.rd32(p), u, 0, 0, d ? e.rd32(d + 0x28) : 0, 0xa);
    }
    return 1;
}
}
