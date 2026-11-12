// 1.13c D2Game+0x76A10: monster mode callback "update get-hit animation" (table entry). ecx = game, edx = the
// monster, no stack words. The monster's animation step runs (0xE1930: eax = monster), its frame is advanced
// (0xB08A: monster) and the end-of-animation test is read (0xB084: monster). Non-zero: the recovery step runs
// (0x765E0: game, monster) and the result is 2. Zero: 1.
#pragma once
#include <cstdint>

namespace g76a10 {
template <class E> uint32_t Update(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xE1930, unit);
    e.h(0xB08A, 0, 0, 0, 0, 0, unit);
    if (!e.h(0xB084, 0, 0, 0, 0, 0, unit)) return 1;
    e.h(0x765E0, 0, game, unit);
    return 2;
}
}
