// 1.13c D2Game+0xE69E0 -- ECX = quest record, EDX = event. No +8 item in the event returns 0; an item whose +4 word is not 0xff, or a +4 byte other than 1 in the record's +0x18 data, returns the item. Otherwise the record's +0x14 byte becomes 0 and its +0xB byte 2, the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xE5D20, EDI = the record's +4 game, stack 0), the data byte and the record's +0xA8 word are cleared, and the worker runs again with the callback 0xE3A80; its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe69e0(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t it = e.rd32(ev + 8);
    if (!it) return 0;
    if ((e.rd32(it + 4) & 0xffff) != 0xff) return it;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 4) != 1) return it;
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 2);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe5d20, 0, 0); e.j(0xDFCC0, e.rd32(q + 4));
    e.wr8(d + 4, 0);
    e.wr32(q + 0xa8, 0);
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe3a80, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return v;
}
}
