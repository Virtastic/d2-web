// 1.13c D2Game+0xE6470 -- ECX = game, EDX = unit, one ignored stack argument: with the unit's quest-flag buffer for the game's difficulty (player data from the D2Common import 0xA4C0), only when the import 0xA568(buffer, 0x11, 1) answers exactly 1: the import 0xA4D2(unit, 4, 5, 0) runs, the worker 0x34B20 runs with 0xF on the stack and EAX = unit, and the import 0xA586(buffer, 0x11, 1) clears the state. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe6470(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x11, 1) != 1) return 0;
    e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 4, 5, 0);
    e.h(0x34B20, unit, 0, 0, 0, 0, 0xf);
    e.h(0xA586, 0, 0, 0, 0, 0, buf, 0x11, 1);
    return 0;
}
}
