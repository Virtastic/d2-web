// 1.13c D2Game+0x5DDF0 ACT5Q3_OnPlayerMessage20F: ECX = game, EDX = unit, one unused stack word (ret 4); returns 0 or 1.
// 0 unless the unit is set and its +4 class is 0x20F. A set node in the unit's pdata+0x30 list (as 0x5DD00) calls
// 0x48410(game, unit, module+0x59180) and returns 1. Otherwise Ordinal 11103(unit, 0), 0x75E10(game, &block, 1) with the
// 0x5DD00 block, and state+0xE3 = 1 on the game's type-0x21 quest node when there is one; returns 1.
#pragma once
#include <cstdint>
#include "d2game_5dd00.h"

namespace d2g5ddf0 {
template <class E> uint32_t Run(E& e, uint32_t game, uint32_t unit, uint32_t mod)
{
    if (!unit || e.rd32(unit + 4) != 0x20F) return 0;
    if (d2g5dd00::FirstSet(e, unit)) { e.h(0x48410, 0, 0, 0, 0, 0, game, unit, mod + 0x59180); return 1; }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    d2g5dd00::Block(e, game, unit);
    const uint32_t lst = e.rd32(game + 0x10F4);
    if (!lst) return 1;
    uint32_t n = e.rd32(lst);
    while (n && e.rd32(n) != 0x21) n = e.rd32(n + 0xF4);
    if (n) e.wr8(e.rd32(n + 0x18) + 0xE3, 1);
    return 1;
}
}
