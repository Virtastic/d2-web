// 1.13c D2Game+0x76700: monster mode callback "reset to neutral with delay" (table entry). ecx = game, edx = the
// mode argument block whose word 1 is the monster, no stack words. The attacker list is released (0xDA750: edi =
// monster; game), the monster goes to mode 11 (0xA4A2: monster, 11) and an event of type 2 is queued 15 frames
// ahead (0x8EA80: game, monster, 2, frame + 15, 0, 0, 0). The result is 1.
#pragma once
#include <cstdint>

namespace g76700 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t p)
{
    const uint32_t unit = e.rd32(p + 4);
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.j(0xDA750, unit);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 11);
    e.h(0x8EA80, 0, game, unit, 0, 0, 2, e.rd32(game + 0xA8) + 15, 0, 0, 0);
    return 1;
}
}
