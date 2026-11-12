// 1.13c D2Game+0x74C30: monster mode-start callback "run" (table entry). ecx = game, edx = the mode block (+0x04
// the monster). The monster's path (+0x2C) is asked whether it can move (0xAC70); if not the result is 0.
// Otherwise the attacker list is dropped (0xDA750: one stack word = game), the monster goes to animation mode 15
// (0xA4A2: unit, mode) and the result is 1.
#pragma once
#include <cstdint>

namespace g74c30 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t blk)
{
    const uint32_t unit = e.rd32(blk + 4);
    if (!e.h(0xAC70, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C))) return 0;
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 15);
    return 1;
}
}
