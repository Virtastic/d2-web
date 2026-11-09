// 1.13c D2Game+0x5E000 ACT5Q5_ReleaseSlotById: EBX = game, EDI = unit id; returns AL. Looks the id up in the game's type-1 unit hash
// (+0x1320 + (id & 0x7F)*4, chain +0xE4, id +0xC); a miss returns 0. A hit with 10494(u, 0x36) set runs 0xE2240 (ECX u, EDX 0);
// 10331(u) == 0 returns 1, else 11103(u, 0), 0x75E10(game, block {0xC, u, 0,0,0, 0x6400, 0,0}, 1), 10885(10331(u), 1, id),
// 10408(u), 11016(u) and returns 1. game == 0 is retail's fatal-error exit (never framed).
#pragma once
#include <cstdint>
#include "d2game_5dd00.h"

namespace d2g5e000 {
template <class E> uint32_t Run(E& e, uint32_t game, uint32_t id)
{
    if (!game) return 0;   // retail: Fog error and exit
    uint32_t u = e.rd32(game + 0x1320 + (id & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    if (!u) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, u, 0x36)) e.h(0xE2240, 0, u, 0);
    if (!e.h(0xA478, 0, 0, 0, 0, 0, u)) return 1;
    e.h(0xA496, 0, 0, 0, 0, 0, u, 0);
    d2g5dd00::Block(e, game, u);
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, u);
    e.h(0xA44E, 0, 0, 0, 0, 0, a, 1, id);
    e.h(0xA550, 0, 0, 0, 0, 0, u);
    e.h(0xA63A, 0, 0, 0, 0, 0, u);
    return 1;
}
}
