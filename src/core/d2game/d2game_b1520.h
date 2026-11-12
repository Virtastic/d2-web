// 1.13c D2Game+0xB1520: AI target callback "nearest reachable high-threat target" (table entry). ecx as
// entered, edx = the asking unit, stack: the candidate, a pointer to three words (best unit, best distance, the
// unit to leave out) (ret 8). A candidate other than the left-out unit that passes the hittable test (0xB13B0:
// ecx as entered, edx = candidate; stack: ecx as entered, asker) has its distance measured (0xB0A20: ebx =
// candidate; stack: asker). At most 0x30 away, with a threat class above 1 (0xB08A0: ecx = candidate), nearer
// than the best so far (signed) and with a clear approach (0xB0E90: eax = candidate, ecx = asker), it becomes
// the best unit and its distance the best distance. The result is always 0.
#pragma once
#include <cstdint>

namespace gb1520 {
template <class E> uint32_t Pick(E& e, uint32_t ecx, uint32_t asker, uint32_t cand, uint32_t p)
{
    if (cand == e.rd32(p + 8)) return 0;
    if (!e.h(0xB13B0, 0, ecx, cand, 0, 0, ecx, asker)) return 0;
    const int32_t dist = (int32_t)e.h(0xB0A20, 0, 0, 0, cand, 0, asker);
    if (dist > 0x30) return 0;
    if ((int32_t)e.h(0xB08A0, 0, cand) <= 1) return 0;
    if (dist >= (int32_t)e.rd32(p + 4)) return 0;
    if (!e.h(0xB0E90, cand, asker)) return 0;
    e.wr32(p, cand);
    e.wr32(p + 4, (uint32_t)dist);
    return 0;
}
}
