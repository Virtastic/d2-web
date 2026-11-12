// 1.13c D2Game+0x74C90: monster mode-start callback (table entry). ecx = game, edx = the mode block (+0x04 the
// monster). Drops the monster's attacker list (0xDA750: one stack word = game, same registers), then puts the
// monster in animation mode 10 (0xA4A2: unit, mode). Result 1.
#pragma once
#include <cstdint>

namespace g74c90 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t blk)
{
    const uint32_t unit = e.rd32(blk + 4);
    e.h(0xDA750, 0, game, blk, 0, 0, game);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 10);
    return 1;
}
}
