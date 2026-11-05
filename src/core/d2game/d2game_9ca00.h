// Nearest same-class monster scan callback: ECX unused, EDX = unit, stack = candidate, best {unit, distance} (ret 8); returns 0.
// A candidate of type 1 other than the unit whose class (0x18A0, EAX = unit) matches the unit's, that #A688 does not exclude and whose
// distance 0x1900 (EAX = unit, EBX = candidate) is below best[1] (signed) becomes best {candidate, distance}.
#pragma once
#include <cstdint>

namespace g9ca00 {
template <class E> uint32_t Scan(E& e, uint32_t unit, uint32_t cand, uint32_t best)
{
    // 1.13c D2Game+0x9CA00
    if (!cand || e.rd32(cand) != 1 || unit == cand) return 0;
    const uint32_t a = e.h(0x18A0, unit), b = e.h(0x18A0, cand);
    if (a != b) return 0;
    if (e.h(0xA688, 0, 0, 0, 0, 0, cand)) return 0;
    const int32_t d = (int32_t)e.h(0x1900, unit, 0, 0, cand);
    if (d < (int32_t)e.rd32(best + 4)) { e.wr32(best, cand); e.wr32(best + 4, (uint32_t)d); }
    return 0;
}
}
