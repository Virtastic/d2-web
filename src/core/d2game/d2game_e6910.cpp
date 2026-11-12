// 1.13c D2Game+0xE6910 -- ECX = quest record q, EDX = event. No +8 item returns 0; an item whose +4 word is not 0xFF, or a q data (+0x18) byte +3 other than 1, returns the item. Otherwise q +0x14 becomes 0 and +0xB 2, the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xE5BE0, EDI = the +4 game, stack 0), the data byte +3 and the q +0xA8 word are cleared and 0 is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe6910(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t it = e.rd32(ev + 8);
    if (!it) return 0;
    if ((e.rd32(it + 4) & 0xffff) != 0xff) return it;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 3) != 1) return it;
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 2);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe5be0, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    e.wr8(d + 3, 0);
    e.wr32(q + 0xa8, 0);
    return 0;
}
}
