// 1.13c D2Game+0xB0E30: AI target callback "dying monster in range" (table entry). ecx and edx as entered, stack:
// the candidate unit, a pointer to the range (ret 8). Only a monster (type 1 at +0x00) in mode 0 (+0x10)
// qualifies. Its distance is measured (0xB0A20: ecx as entered, ebx = the candidate, one stack word = edx) and the
// candidate comes back when that is below the range (unsigned); otherwise 0.
#pragma once
#include <cstdint>

namespace gb0e30 {
template <class E> uint32_t Pick(E& e, uint32_t ecx, uint32_t edx, uint32_t unit, uint32_t pRange)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    if (e.rd32(unit + 0x10) != 0) return 0;
    const uint32_t dist = e.h(0xB0A20, 0, ecx, 0, unit, 0, edx);
    return dist < e.rd32(pRange) ? unit : 0;
}
}
