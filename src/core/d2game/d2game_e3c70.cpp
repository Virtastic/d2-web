// 1.13c D2Game+0xE3C70 -- ECX = game, EDX = unit, one ignored stack argument. The list at the game's +0x10F4 holder (first word = head; links at +0xF4) is walked for the node whose first word is 0x12; a missing holder, head or node returns the holder word. For that node, with the unit's quest-flag buffer (0xA4C0 player data, byte +0x6D, buffers from +0x10): a non-zero 0xA568(buffer, 0x14, 0) or 0xA568(buffer, 0x14, 1) is returned, else 0xA57A(buffer, 0x14, the node's +0x18 record word +0x18) and its answer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe3c70(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (!holder) return holder;
    uint32_t n = e.rd32(holder);
    if (!n) return holder;
    while (e.rd32(n) != 0x12) { n = e.rd32(n + 0xf4); if (!n) return holder; }
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x14, 0);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x14, 1);
    if (v) return v;
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x14, e.rd32(e.rd32(n + 0x18) + 0x18));
}
}
