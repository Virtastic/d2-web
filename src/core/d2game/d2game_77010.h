// 1.13c D2Game+0x77010: monster mode update callback "walk with trails" (table entry). ecx = game, edx = the
// monster. A monster carrying state 13 (0xA68E: monster, 13) leaves a first trail (0x439A0: game); one carrying
// state 22 (0xA68E: monster, 22) leaves a second (0xB4870: game). The path then advances (0xE1930: eax = monster):
// a result of 2 means the walk ended, the end-of-walk step runs (0x76BF0: ecx = game, edx = monster) and the
// result is 2; anything else gives 1.
#pragma once
#include <cstdint>

namespace g77010 {
template <class E> uint32_t Update(E& e, uint32_t game, uint32_t unit)
{
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 13)) e.h(0x439A0, 0, 0, 0, 0, 0, game);
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 22)) e.h(0xB4870, 0, 0, 0, 0, 0, game);
    if (e.h(0xE1930, unit) != 2) return 1;
    e.h(0x76BF0, 0, game, unit);
    return 2;
}
}
