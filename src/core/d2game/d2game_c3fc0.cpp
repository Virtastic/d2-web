// 1.13c D2Game+0xC3FC0 -- ECX = game, EDX = unit, one ignored stack argument: the D2Common import 0xA4C0(unit) gives the player data, whose quest-flag buffer for the game's difficulty (byte +0x6D; buffers from +0x10) is used. If the import 0xA568(buffer, 0xA, 0) answers 1, or 0xA568(buffer, 0xA, 3) answers non-zero, nothing more happens; otherwise the game's quest list (holder at +0x10F4; node id +0, next +0xF4) is searched for id 9 and the +0x14 counter of that node's +0x18 data is incremented. Returns 0 on every path.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3fc0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xa, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xa, 3) != 0) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 9) n = e.rd32(n + 0xf4);
    if (n) { const uint32_t d = e.rd32(n + 0x18); e.wr32(d + 0x14, e.rd32(d + 0x14) + 1); }
    return 0;
}
}
