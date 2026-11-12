// 1.13c D2Game+0x5D920 -- ECX = quest record, EDX = event. No +8 item in the event returns 0; an item whose +4 word is not 0x209 returns the item. A non-zero +0x98 byte in the record's +0x18 data returns the item pointer with its low byte replaced by that byte. Otherwise the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0x5D760, EDI = the record's +4 game, stack = the event's +0xC unit), the data byte becomes 1 and the worker's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f5d920(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t it = e.rd32(ev + 8), d = e.rd32(q + 0x18);
    if (!it) return 0;
    if ((e.rd32(it + 4) & 0xffff) != 0x209) return it;
    const uint32_t b = e.rd8(d + 0x98);
    if (b) return (it & ~0xffu) | b;
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x5d760, 0, e.rd32(ev + 0xc));
    e.j(0xDFCC0, e.rd32(q + 4));
    e.wr8(d + 0x98, 1);
    return v;
}
}
