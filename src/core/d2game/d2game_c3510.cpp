// 1.13c D2Game+0xC3510 -- ECX = game, EDX = unit, one ignored stack argument: the D2Common import 0xA4C0(unit) gives the player data, whose quest-flag buffer for the game's difficulty (byte +0x6D; buffers from +0x10) is used. If the import 0xA568(buffer, 0xc, 0) or 0xA568(buffer, 0xc, 0x1) answers 1 nothing more happens; otherwise 0xA57A(buffer, 0xc, 0xe) sets the state. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3510(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xc, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xc, 0x1) == 1) return 0;
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xc, 0xe);
    return 0;
}
}
