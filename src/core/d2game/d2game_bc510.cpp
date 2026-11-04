// 1.13c D2Game+0xBC510 -- shift the quest record's two coordinates and refresh (ECX = context: +0 game, +4 unit).
// Written from the 1.13c disassembly as a rule over the seam environment. With no unit, or a unit whose +0x10 is 0,
// find the type-0x17 node in the game's list (game+0x10F4 then head, chain +0xF4), take the pair at node data + 0x2C,
// ask 0xA544(unit, &pair), add (-0x27, 0x21) to the pair and call 0xBC050 (EAX = &pair, stack = context, 3). Always returns 0.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fbc510(GE& e, uint32_t c)
{
    const uint32_t q = e.rd32(c + 4);
    if (q && e.rd32(q + 0x10)) return 0;
    const uint32_t lst = e.rd32(e.rd32(c) + 0x10F4);
    if (!lst) return 0;
    uint32_t n = e.rd32(lst);
    if (!n) return 0;
    while (e.rd32(n) != 0x17) { n = e.rd32(n + 0xF4); if (!n) return 0; }
    const uint32_t d = e.rd32(n + 0x18), s = d + 0x2C;
    e.h(0xA544, 0, 0, 0, 0, 0, q, s);
    e.wr32(s, e.rd32(s) + (uint32_t)(-0x27));
    e.wr32(s + 4, e.rd32(s + 4) + (uint32_t)(0x21));
    e.h(0xBC050, s, 0, 0, 0, 0, c, 3);
    return 0;
}
}
