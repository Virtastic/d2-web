// 1.13c D2Game+0x74870: unit timer callback (table entry). ecx = game, edx = the unit, two stack words that are
// overwritten. Hands over to the stat-list expiry step (0xAA2A: unit, the game frame at game+0xA8); its result is
// the result.
#pragma once
#include <cstdint>

namespace g74870 {
template <class E> uint32_t ExpireStats(E& e, uint32_t game, uint32_t unit)
{
    return e.h(0xAA2A, 0, 0, 0, 0, 0, unit, e.rd32(game + 0xA8));
}
}
