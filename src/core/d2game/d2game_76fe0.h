// 1.13c D2Game+0x76FE0: monster mode-update callback "run" (table entry). ecx = game, edx = the monster. The
// movement step (0xE1930: eax and esi = the monster, ecx and edi = game, edx = the monster) reports its state; on
// state 2 the mode-end handler runs (0x76BF0: ecx = game, edx = the monster) and the result is 2. Otherwise 1.
#pragma once
#include <cstdint>

namespace g76fe0 {
template <class E> uint32_t Update(E& e, uint32_t game, uint32_t unit)
{
    const uint32_t st = e.h(0xE1930, unit, game, unit, 0, unit);
    e.j(0xE1930, game);
    if (st != 2) return 1;
    e.h(0x76BF0, 0, game, unit);
    return 2;
}
}
