// 1.13c D2Game+0x61C00 -- ECX = quest record, EDX = event. The helper 0xE660 (EAX = the event's +0xC unit, EBX = its +0 game, stack 0x206D6468) answering 0 returns 0. A zero +9 record byte or a +0xC byte of 5 returns the helper's answer with AL = the +9 byte. Otherwise the +0x9C counter of the record's +0x18 data is incremented; when it becomes 1, the +1 data byte is set and the +0x98 data word is 2, the record's +0xB byte becomes 2. The data pointer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f61c00(GE& e, uint32_t q, uint32_t ev)
{
    uint32_t v = e.h(0xE660, e.rd32(ev + 0xc), 0, 0, e.rd32(ev), 0, 0x206d6468);
    if (!v) return 0;
    const uint32_t b9 = e.rd8(q + 9);
    if (!b9 || e.rd8(q + 0xc) == 5) return (v & ~0xffu) | b9;
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t n = e.rd32(d + 0x9c) + 1;
    e.wr32(d + 0x9c, n);
    if (n == 1 && e.rd8(d + 1) && e.rd32(d + 0x98) == 2) e.wr8(q + 0xb, 2);
    return d;
}
}
