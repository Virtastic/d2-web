// 1.13c D2Game+0x23650: unique-monster modifier callback (table entry). ecx = game, edx = unit, stack = (a, b),
// ret 8. With b set and the unit a missile (type 3) the elemental-damage step (0x22B60, same registers and stack)
// takes over and its result is the result; otherwise nothing happens and EAX holds b.
#pragma once
#include <cstdint>

namespace g23650 {
template <class E> uint32_t MissileElement(E& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    if (b && unit && e.rd32(unit) == 3) return e.h(0x22B60, 0, game, unit, 0, 0, a, b);
    return b;
}
}
