// 1.13c D2Game+0x759F0: monster mode update callback (table entry). ecx = game, edx = the monster. When the
// animation check passes (0xB07E: monster, 0; non-zero) the animation advances (0xE1930: eax = monster; 0xB08A:
// monster) and, unless it has now ended (0xB084: monster, non-zero), nothing more happens. A failed check or an
// ended animation frees the attacker list (0xDA750: edi = monster; stack: game) and puts the monster in mode 12
// (0xA4A2: monster, 12). The result is always 1.
#pragma once
#include <cstdint>

namespace g759f0 {
template <class E> uint32_t Update(E& e, uint32_t game, uint32_t unit)
{
    if (e.h(0xB07E, 0, 0, 0, 0, 0, unit, 0)) {
        e.h(0xE1930, unit);
        e.h(0xB08A, 0, 0, 0, 0, 0, unit);
        if (!e.h(0xB084, 0, 0, 0, 0, 0, unit)) return 1;
    }
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.j(0xDA750, unit);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 12);
    return 1;
}
}
