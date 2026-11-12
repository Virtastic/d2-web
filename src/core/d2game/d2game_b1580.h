// 1.13c D2Game+0xB1580: nearest-target search callback (table entry). ecx = the game, edx = the searching unit,
// two stack words (ret 8): the candidate and the search record (best unit at +0, best distance at +4, the unit to
// leave out at +8). Always returns 0. The candidate is passed over when it is the left-out unit, when it fails
// the validity test (0xB1400: the game on the stack, esi = candidate, edi = searcher), when its distance
// (0xB0A20: the searcher on the stack, ebx = candidate) is over 0x30 or not below the best so far, when its count
// (0xB08A0: ecx = candidate) is 1 or less, or when the line test (0xAD78: candidate, searcher, 4) is non-zero.
// Otherwise it becomes the best.
#pragma once
#include <cstdint>

namespace gb1580 {
template <class E> uint32_t Visit(E& e, uint32_t game, uint32_t self, uint32_t cand, uint32_t rec)
{
    if (cand == e.rd32(rec + 8)) return 0;
    const uint32_t ok = e.h(0xB1400, 0, 0, 0, 0, cand, game);
    e.j(0xB1400, self);
    if (!ok) return 0;
    const int32_t dist = (int32_t)e.h(0xB0A20, 0, 0, 0, cand, 0, self);
    if (dist > 0x30) return 0;
    if ((int32_t)e.h(0xB08A0, 0, cand) <= 1) return 0;
    if (dist >= (int32_t)e.rd32(rec + 4)) return 0;
    if (e.h(0xAD78, 0, 0, 0, 0, 0, cand, self, 4)) return 0;
    e.wr32(rec, cand);
    e.wr32(rec + 4, (uint32_t)dist);
    return 0;
}
}
