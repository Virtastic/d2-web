// 1.13c D2Game+0xE5370 -- ECX = game, EDX = unit, one ignored stack argument. Unless 0xA568(buffer, 0x11, 0) or 0xA568(buffer, 0x11, 1) answers 1 (buffer = the unit's quest-flag buffer for the game's difficulty: 0xA4C0 player data, byte +0x6D, buffers from +0x10), the list at the game's +0x10F4 holder (first word = head; links at +0xF4) is walked for the node whose first word is 0xF; a node with state byte (+0xC) 2 or 3 sets 0xA57A(buffer, 0x11, 2). Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe5370(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x11, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x11, 1) == 1) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (!holder) return 0;
    uint32_t n = e.rd32(holder);
    if (!n) return 0;
    while (e.rd32(n) != 0xf) { n = e.rd32(n + 0xf4); if (!n) return 0; }
    const uint32_t st = e.rd8(n + 0xc);
    if (st == 2 || st == 3) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x11, 2);
    return 0;
}
}
