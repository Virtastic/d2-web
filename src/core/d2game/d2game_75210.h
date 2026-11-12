// 1.13c D2Game+0x75210: monster mode start callback (table entry). ecx = game, edx = a block whose +0x04 is the
// monster. No monster, or a monster in mode 0 or 12 (+0x10): 1. Otherwise the class check runs (0x1560: eax = the
// monster's class at +0x04, edx = 3); zero ends with 0. Non-zero: the attacker list is freed (0xDA750: edi =
// monster; stack: game), the monster goes to mode 3 (0xA4A2: monster, 3) and the result is 1.
#pragma once
#include <cstdint>

namespace g75210 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t blk)
{
    const uint32_t unit = e.rd32(blk + 4);
    if (!unit) return 1;
    const uint32_t mode = e.rd32(unit + 0x10);
    if (mode == 0 || mode == 12) return 1;
    if (!e.h(0x1560, e.rd32(unit + 4), 0, 3)) return 0;
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.j(0xDA750, unit);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 3);
    return 1;
}
}
