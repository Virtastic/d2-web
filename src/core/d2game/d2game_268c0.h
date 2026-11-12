// 1.13c D2Game+0x268C0: unique-monster modifier callback (table entry). ecx = game, edx = unit, stack = (a, b),
// ret 8. With b set, and the unit absent or not in mode 3 (+0x10), the lightning step (0x26320, same registers and
// stack) takes over and its result is the result; otherwise nothing happens and EAX holds b.
#pragma once
#include <cstdint>

namespace g268c0 {
template <class E> uint32_t LightningUnlessMode3(E& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    if (!b) return 0;
    if (unit && e.rd32(unit + 0x10) == 3) return b;
    return e.h(0x26320, 0, game, unit, 0, 0, a, b);
}
}
