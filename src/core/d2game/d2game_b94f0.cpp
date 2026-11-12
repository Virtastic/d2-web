// 1.13c D2Game+0xB94F0 -- ECX = game, EDX = unit, one unused stack argument; the quest-flag buffer is the unit's player data buffer for the game's difficulty. The import 0xA568(buffer, 0x19, 0) answering 1, or (buffer, 0x19, 1) answering non-zero, returns that answer. The D2Common imports 0xA478(unit), 0xA556(room) and 0xA56E(level) follow; a zero room or level returns 0 and a low byte of the last answer other than 3 returns it. Otherwise 0xA57A(buffer, 0x19, 0xD) and (buffer, 0x19, 1) set the states and the result of the import 0xA574(buffer, 0x19) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb94f0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x19, 0x0);
    if (v == 1) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x19, 0x1);
    if (v) return v;
    v = e.h(0xa478, 0, 0, 0, 0, 0, unit);
    if (!v) return 0;
    v = e.h(0xa556, 0, 0, 0, 0, 0, v);
    if (!v) return 0;
    v = e.h(0xa56e, 0, 0, 0, 0, 0, v);
    if ((v & 0xff) != 3) return v;
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x19, 0xd);
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x19, 0x1);
    return e.h(0xA574, 0, 0, 0, 0, 0, buf, 0x19);
}
}
