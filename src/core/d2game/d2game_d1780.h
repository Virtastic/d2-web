// 1.13c D2Game+0xD1780: item timer callback (table entry). ecx = game, edx = the item. Calls the stat-list expiry
// step (0xAA2A: item, the game frame at game+0xA8); its result is the result.
#pragma once
#include <cstdint>

namespace gd1780 {
template <class E> uint32_t ExpireStats(E& e, uint32_t game, uint32_t item)
{
    return e.h(0xAA2A, 0, 0, 0, 0, 0, item, e.rd32(game + 0xA8));
}
}
