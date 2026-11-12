// 1.13c D2Game+0xC1B0: skill start callback "self resurrect" (table entry). ecx = game, edx = the unit, two stack
// words unread (ret 8). Anything but a monster (type 1 at +0x00): 0. A monster is re-attached (0xB33B0: eax and
// ecx = game, edx = 1, esi = the unit), loses flag bits 1..3 at +0xC4, has its path flag 0x100 applied (0xA682:
// path at +0x2C, 0x100) and the result is 1.
#pragma once
#include <cstdint>

namespace gc1b0 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t unit)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    e.h(0xB33B0, game, game, 1, 0, unit);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFF1u);
    e.h(0xA682, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C), 0x100);
    return 1;
}
}
