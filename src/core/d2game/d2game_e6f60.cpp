// 1.13c D2Game+0xE6F60 -- ECX = quest record q, EDX = event (+0 unit, +0xC game). The test 0xE660 (EAX = game, EBX = unit, stack 0x20626262) answering 0 returns 0. Otherwise the data (q+0x18) counter +8 is incremented and, unless it became 1, returned. With q +9 and the data byte +5 set: data +5 is cleared, q +0x14 becomes 0 and +0xB 2, and the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xE6550, EDI = the +4 game, stack 0) and its result is returned; otherwise 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe6f60(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t game = e.rd32(ev + 0xc), unit = e.rd32(ev), d = e.rd32(q + 0x18);
    if (!e.h(0xE660, game, 0, 0, unit, 0, 0x20626262)) return 0;
    const uint32_t n = e.rd32(d + 8) + 1;
    e.wr32(d + 8, n);
    if (n != 1) return n;
    if (e.rd8(q + 9) == 0 || e.rd8(d + 5) == 0) return 0;
    e.wr8(d + 5, 0);
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 2);
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe6550, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return v;
}
}
