// 1.13c D2Game+0xC3EF0 -- ECX = game, EDX = unit, one ignored stack argument: the worker 0xE660 (EAX = unit, EBX = game) is asked for the item code 0x20786F62; a non-zero answer ends the call. Otherwise the game's quest list (holder at +0x10F4; node id +0, next +0xF4) is searched for id 9 and the +0x14 counter of that quest's +0x18 data is incremented. Returns 0 on every path.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3ef0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    if (e.h(0xE660, unit, 0, 0, game, 0, 0x20786f62)) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 9) n = e.rd32(n + 0xf4);
    if (n) { const uint32_t d = e.rd32(n + 0x18); e.wr32(d + 0x14, e.rd32(d + 0x14) + 1); }
    return 0;
}
}
