// 1.13c D2Game+0x626C0 -- ECX = game, EDX = unit, one ignored stack argument: with the unit's quest-flag buffer for the game's difficulty (player data from the D2Common import 0xA4C0), the imports 0xA568(buffer, 6, 1) and 0xA568(buffer, 6, 0xD) are asked in turn; when both answer non-zero the worker 0xDFFD0 runs with (0x21, unit), EAX = the second answer and ESI = unit. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f626c0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, buf, 6, 1)) return 0;
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 6, 0xd);
    if (!v) return 0;
    e.h(0xDFFD0, v, 0, 0, 0, unit, 0x21, unit);
    return 0;
}
}
