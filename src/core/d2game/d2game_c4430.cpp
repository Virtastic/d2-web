// 1.13c D2Game+0xC4430 -- ECX = game, EDX = unit, one ignored stack word: like 0xC3550 for state 9 and the kind 8 node of the list at game+0x10F4. Two 0xA568 tests (0, then 1) of state 9 end it when either is 1. With the node found: byte +0xC == 2 sets state 2 (0xA57A); +0xC == 3 sets state 3 when byte +0xB is 1, else state 4. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4430(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 9, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 9, 1) == 1) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (!holder) return 0;
    uint32_t n = e.rd32(holder);
    while (n && e.rd32(n) != 8) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    const uint32_t cl = e.rd8(n + 0xc);
    if (cl == 2) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 9, 2);
    else if (cl == 3) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 9, e.rd8(n + 0xb) == 1 ? 3 : 4);
    return 0;
}
}
