// 1.13c D2Game+0x592F0 -- ECX = game, EDX = unit, one ignored stack argument; the quest-flag buffer is the unit's player data (D2Common import 0xA4C0) buffer for the game's difficulty (byte +0x6D). When the import 0xA568(buffer, 0x24, 0xD) answers non-zero: the unit's +0x6E word becomes 0x51, its +0x70 word the unit itself, the D2Common import 0xA4DE(unit) runs and bit 0x400 of the unit's +0xC4 word is set. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f592f0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x24, 0xd)) return 0;
    e.wr8(unit + 0x6e, 0x51);
    e.wr8(unit + 0x6f, 0x0);
    e.wr32(unit + 0x70, unit);
    e.h(0xa4de, 0, 0, 0, 0, 0, unit);
    e.wr32(unit + 0xc4, e.rd32(unit + 0xc4) | 0x400);
    return 0;
}
}
