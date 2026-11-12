// 1.13c D2Game+0x759B0: monster mode callback "recover to neutral" (table entry). ecx = game, edx = the monster,
// no stack words. The monster's animation step runs (0xE1930: eax = monster), its frame is advanced (0xB08A:
// monster) and the end-of-animation test is read (0xB084: monster). When that is non-zero the attacker list is
// released (0xDA750: edi = monster; game) and the monster goes to mode 11 (0xA4A2: monster, 11). The result is 1.
#pragma once
#include <cstdint>

namespace g759b0 {
template <class E> uint32_t Update(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xE1930, unit);
    e.h(0xB08A, 0, 0, 0, 0, 0, unit);
    if (e.h(0xB084, 0, 0, 0, 0, 0, unit)) {
        e.h(0xDA750, 0, 0, 0, 0, 0, game);
        e.j(0xDA750, unit);
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 11);
    }
    return 1;
}
}
