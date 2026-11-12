// 1.13c D2Game+0x57440 -- ECX = game, EDX = unit, one unused stack argument. Only when the unit's room (D2Common import 0xA478) is in level 0x78 (import 0xA556), the unit is not null and its +0x10 mode is neither 0 nor 0x11: the game's +0x10F4 record list is walked for the record with id 0x23 and the +0x54 counter of its +0x18 data is incremented (retail faults when the list has no such record). Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f57440(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t room = e.h(0xa478, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    if (e.h(0xa556, 0, 0, 0, 0, 0, room) != 0x78) return 0;
    if (!unit) return 0;
    const uint32_t m = e.rd32(unit + 0x10);
    if (!m || m == 0x11) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0x23) n = e.rd32(n + 0xf4);
    if (!n) return 0;   // retail reads through null here
    const uint32_t d = e.rd32(n + 0x18);
    e.wr32(d + 0x54, e.rd32(d + 0x54) + 1);
    return 0;
}
}
