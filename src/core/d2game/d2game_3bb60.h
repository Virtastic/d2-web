// 1.13c D2Game+0x3BB60: missile hit callback (table entry). ecx = game, edx = the missile, one stack word = the
// unit hit (ret 4). A unit hit does nothing (0). A wall hit hands over to the area step (0x3B5F0, same registers,
// eax = 0, stack word 0); its result is the result.
#pragma once
#include <cstdint>

namespace g3bb60 {
template <class E> uint32_t WallHit(E& e, uint32_t game, uint32_t missile, uint32_t target)
{
    if (target) return 0;
    return e.h(0x3B5F0, 0, game, missile, 0, 0, 0);
}
}
