// 1.13c D2Game+0x616F0 -- ECX = game, EDX = unit, one ignored stack argument; the quest-flag buffer is the unit's player data (D2Common import 0xA4C0) buffer for the game's difficulty (byte +0x6D). Each of the imports 0xA568(buffer, 0x4, s), s = 0x0, 0x1, answering 1 returns 1. The D2Common imports 0xA478(unit) (room), 0xA556(room) (level) and 0xA56E(level) follow; a zero room or level returns 0 and a low byte of the last answer other than 0x0 returns that answer. Otherwise 0xA57A(buffer, 0x4, s) sets s = 0x4 and the last setter's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f616f0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x4, 0x0) == 1) return 1;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x4, 0x1) == 1) return 1;
    uint32_t v = e.h(0xa478, 0, 0, 0, 0, 0, unit);
    if (!v) return 0;
    v = e.h(0xa556, 0, 0, 0, 0, 0, v);
    if (!v) return 0;
    v = e.h(0xa56e, 0, 0, 0, 0, 0, v);
    if ((v & 0xff) != 0x0) return v;
    v = e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x4, 0x4);
    return v;
}
}
