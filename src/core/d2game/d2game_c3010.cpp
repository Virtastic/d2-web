// 1.13c D2Game+0xC3010 -- ECX = game, EDX = unit, one unused stack argument; the quest-flag buffer is the unit's player data buffer for the game's difficulty. Unless the import 0xA568(buffer, 0xD, 0) or (buffer, 0xD, 1) answers 1: when the game's +0x10F4 record list has a record with id 0xC whose +0xC byte is 1, 0xA57A(buffer, 0xD, 2) runs. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3010(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xd, 0x0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xd, 0x1) == 1) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0xc) n = e.rd32(n + 0xf4);
    if (n && e.rd8(n + 0xc) == 1) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xd, 0x2);
    return 0;
}
}
