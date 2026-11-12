// 1.13c D2Game+0x636C0 -- EAX = whatever the caller left, ECX = quest record. A zero +9 byte, or a +0xC byte of 5, returns EAX with AL = the +9 byte. Otherwise the +0x9C counter of the record's +0x18 data is decremented; a non-zero counter, a zero +1 data byte or a +0x98 data word other than 2 returns the data pointer. Otherwise the record's +0x14 byte becomes 0, its +0xB byte 3 and the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0x62AE0, EDI = the record's +4 game, stack 0); its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f636c0(GE& e, uint32_t a, uint32_t q)
{
    const uint32_t b9 = e.rd8(q + 9);
    if (!b9 || e.rd8(q + 0xc) == 5) return (a & ~0xffu) | b9;
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t n = e.rd32(d + 0x9c) - 1;
    e.wr32(d + 0x9c, n);
    if (n || !e.rd8(d + 1) || e.rd32(d + 0x98) != 2) return d;
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 3);
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x62ae0, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return v;
}
}
