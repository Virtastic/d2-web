// 1.13c D2Game+0x63C40 -- ECX = quest record, EDX = event. No +8 item in the event returns 0; an item whose +4 word is not 0x94 returns the item. With the +0x86 byte of the record's +0x18 data not 1 the item pointer with its low byte replaced by 1 is returned. Otherwise the record's +0xB byte becomes 1 and its +0x14 byte 0, the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0x62D30, EDI = the record's +4 game, stack 0), the data byte and the record's +0xA8 word are cleared and 0 is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f63c40(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t it = e.rd32(ev + 8);
    if (!it) return 0;
    if ((e.rd32(it + 4) & 0xffff) != 0x94) return it;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 0x86) != 1) return (it & ~0xffu) | 1;
    e.wr8(q + 0xb, 1);
    e.wr8(q + 0x14, 0);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x62d30, 0, 0); e.j(0xDFCC0, e.rd32(q + 4));
    e.wr8(d + 0x86, 0);
    e.wr32(q + 0xa8, 0);
    return 0;
}
}
