// 1.13c D2Game+0x5DEF0 ACT5Q3_TickDispatch: EAX = game; void. Finds the type-0x21 quest node (game+0x10F4 list), reads the tracked
// unit id at state+0x9C and looks it up in the node's game (node+4) type-1 unit hash (+0x1320 + (id & 0x7F)*4, chain +0xE4, id +0xC);
// a hit runs 0x5DD00 (ESI = unit, stack node). node+4 == 0 is retail's fatal-error exit (never framed).
#pragma once
#include <cstdint>

namespace d2g5def0 {
template <class E> void Run(E& e, uint32_t game)
{
    const uint32_t lst = e.rd32(game + 0x10F4);
    if (!lst) return;
    uint32_t n = e.rd32(lst);
    while (n && e.rd32(n) != 0x21) n = e.rd32(n + 0xF4);
    if (!n) return;
    const uint32_t id = e.rd32(e.rd32(n + 0x18) + 0x9C), g = e.rd32(n + 4);
    if (!g) return;   // retail: Fog error and exit
    uint32_t u = e.rd32(g + 0x1320 + (id & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    if (u) e.h(0x5DD00, 0, 0, 0, 0, u, n);
}
}
