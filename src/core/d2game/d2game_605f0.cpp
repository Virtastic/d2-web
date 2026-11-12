// 1.13c D2Game+0x605F0 -- ECX = game, EDX = unit, one unused stack argument. When the unit's room (D2Common import 0xA478) is in level 0x26 (import 0xA556) and the game's +0x10F4 record list has a record with id 4, the unit is stored in the +0x74 word of that record's +0x18 data and 1 is returned; 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f605f0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t room = e.h(0xa478, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    if (e.h(0xa556, 0, 0, 0, 0, 0, room) != 0x26) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0x4) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    e.wr32(e.rd32(n + 0x18) + 0x74, unit);
    return 1;
}
}
