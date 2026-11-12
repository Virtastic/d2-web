// 1.13c D2Game+0x57D80 -- ECX = game, EDX = unit, one ignored stack argument: the worker 0xE660 (EAX = unit, EBX = game) is asked for the item code 0x20656369; a zero answer returns 0. Otherwise the game's quest list (holder at +0x10F4; node id +0, next +0xF4) is searched for id 3; the +0xE2 flag of that quest's +0x18 data is set and the data pointer returned, or 0 when the quest is absent.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f57d80(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    if (!e.h(0xE660, unit, 0, 0, game, 0, 0x20656369)) return 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 3) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    const uint32_t d = e.rd32(n + 0x18);
    e.wr8(d + 0xe2, 1);
    return d;
}
}
