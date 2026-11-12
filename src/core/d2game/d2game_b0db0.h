// 1.13c D2Game+0xB0DB0: AI target callback "count living monsters of a class in range" (table entry). edx as
// entered, stack: the candidate unit, a pointer to three words (class, range, count) (ret 8). A monster (type 1 at
// +0x00) of the asked class (+0x04) that is not dead (0xDFA90: eax = candidate) has its distance measured
// (0xB0A20: ebx = whatever the dead check left in edx, which is no longer the edx as entered; stack: candidate);
// at or inside the range (signed) the count goes up by one. The result is always 0. The dead check's edx is not
// modelled here: the seam passes 0, the value a replaced dead check leaves.
#pragma once
#include <cstdint>

namespace gb0db0 {
template <class E> uint32_t Count(E& e, uint32_t /*edx*/, uint32_t unit, uint32_t p)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    if (e.rd32(unit + 4) != e.rd32(p)) return 0;
    if (e.h(0xDFA90, unit)) return 0;
    const int32_t dist = (int32_t)e.h(0xB0A20, 0, 0, 0, 0, 0, unit);
    if (dist <= (int32_t)e.rd32(p + 4)) e.wr32(p + 8, e.rd32(p + 8) + 1);
    return 0;
}
}
