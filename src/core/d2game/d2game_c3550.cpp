// 1.13c D2Game+0xC3550 -- ECX = game, EDX = unit, one ignored stack word: the quest-flag buffer of the unit for the game difficulty (byte +0x6D) is tested with 0xA568 for state 0xC (0, then 1); either answering 1 ends it. Otherwise the kind 0xB node of the list at game+0x10F4 (head at [holder], next at +0xF4) is found; its byte +0xC (2, 3 or 4) and, for 4, byte +0xB (2 or 3 sets state 4, 4 sets state 5) pick the 0xA57A state to set. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3550(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xc, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xc, 1) == 1) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (!holder) return 0;
    uint32_t n = e.rd32(holder);
    while (n && e.rd32(n) != 0xb) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    const uint32_t cl = e.rd8(n + 0xc);
    if (cl == 2) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xc, 2);
    else if (cl == 3) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xc, 3);
    else if (cl == 4) {
        const uint32_t al = e.rd8(n + 0xb);
        if (al == 2 || al == 3) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xc, 4);
        else if (al == 4) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xc, 5);
    }
    return 0;
}
}
