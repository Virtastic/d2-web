// 1.13c D2Game+0xB0DF0: AI target callback "nearest closed door" (table entry). edx as entered, stack: the
// candidate unit, a pointer to two words (best unit, best distance) (ret 8). An object (type 2 at +0x00) that
// passes the door test (0xAD6C: candidate) and is in mode 0 (+0x10) has its squared distance measured (0x1900:
// eax = candidate, ebx = edx as entered); when that is below the best distance (signed) the candidate and its
// distance replace the pair. The result is always 0.
#pragma once
#include <cstdint>

namespace gb0df0 {
template <class E> uint32_t Pick(E& e, uint32_t edx, uint32_t unit, uint32_t p)
{
    if (!unit || e.rd32(unit) != 2) return 0;
    if (!e.h(0xAD6C, 0, 0, 0, 0, 0, unit)) return 0;
    if (e.rd32(unit + 0x10)) return 0;
    const uint32_t dist = e.h(0x1900, unit, 0, 0, edx);
    if ((int32_t)dist < (int32_t)e.rd32(p + 4)) {
        e.wr32(p, unit);
        e.wr32(p + 4, dist);
    }
    return 0;
}
}
