// 1.13c D2Game+0xE6D90 -- ECX = quest record q, EDX = event. No +8 item returns 0; an item whose +4 word is not 0xF5 returns the item; a q data (+0x18) byte +1 other than 1 returns the item with its low byte replaced by 1. Otherwise q +0x14 becomes 0 and +0xB 1, the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0xE6420, EDI = the +4 game, stack 0), q +0xA8 is cleared and the worker result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe6d90(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t it = e.rd32(ev + 8);
    if (!it) return 0;
    if ((e.rd32(it + 4) & 0xffff) != 0xf5) return it;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 1) != 1) return (it & ~0xffu) | 1;
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 1);
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe6420, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    e.wr32(q + 0xa8, 0);
    return v;
}
}
