// 1.13c D2Game+0x61D40 -- ECX = game, EDX = unit, one unused stack argument; the quest-flag buffer is the unit's player data buffer for the game's difficulty. The import 0xA568(buffer, 3, 0) or (buffer, 3, 1) answering 1 returns 1; the D2Common import 0xA3E2(unit, 0xC, 0) below 8 (signed) returns it. Otherwise 0xA57A(buffer, 3, 0xD) runs and the result of 0xA57A(buffer, 3, 1) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f61d40(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x3, 0x0);
    if (v == 1) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x3, 0x1);
    if (v == 1) return v;
    v = e.h(0xA3E2, 0, 0, 0, 0, 0, unit, 0xc, 0);
    if ((int32_t)v < 8) return v;
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x3, 0xd);
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x3, 0x1);
}
}
