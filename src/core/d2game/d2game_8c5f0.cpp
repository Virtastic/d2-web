// 1.13c D2Game+0x8C5F0 -- ECX unused, EDX = unit, one stack argument. For a player unit (non-null, type 0) the D2Common import 0xA4C0(unit) gives the player data and its +0x9C word goes in EAX; otherwise EAX = 0. The worker 0x8A3E0 then runs with (argument, 0x7) and its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f8c5f0(GE& e, uint32_t, uint32_t unit, uint32_t a)
{
    uint32_t v = 0;
    if (unit && !e.rd32(unit)) v = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9c);
    return e.h(0x8A3E0, v, 0, 0, 0, 0, a, 0x7);
}
}
