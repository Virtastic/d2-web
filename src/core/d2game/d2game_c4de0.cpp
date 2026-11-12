// 1.13c D2Game+0xC4DE0 -- ECX = game, EDX = unit, one ignored stack argument: with the unit's quest-flag buffer for the game's difficulty (player data from the D2Common import 0xA4C0), the import 0xA568(buffer, 0xD, 0xD) must answer non-zero, the import 0xA478(unit) must give a room and the import 0xA556(room) must answer 0x4A; then the worker 0xDFFD0 runs with (0x33, unit), EAX = 0x4A and ESI = unit. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4de0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xd, 0xd)) return 0;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    const uint32_t lvl = e.h(0xA556, 0, 0, 0, 0, 0, room);
    if (lvl != 0x4a) return 0;
    e.h(0xDFFD0, lvl, 0, 0, 0, unit, 0x33, unit);
    return 0;
}
}
