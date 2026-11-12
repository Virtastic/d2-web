// 1.13c D2Game+0x61500 -- ECX = game, EDX = unit, one ignored stack argument: the D2Common import 0xA478(unit) gives a room; for a non-null room whose import 0xA556 answers 0x27, the import 0xA4C0(unit) gives the player data and the quest-flag buffer for the game's difficulty goes to the import 0xA57A(buffer, 4, 0xA). Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f61500(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room || e.h(0xA556, 0, 0, 0, 0, 0, room) != 0x27) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 4, 0xa);
    return 0;
}
}
