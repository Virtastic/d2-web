// 1.13c D2Game+0xB3890: skill start callback of the swarm's move (table entry). ecx unread, edx = the unit, two
// stack words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit) and read from its path (0xA598:
// path at +0x2C); none, or the unit itself: 0. The unit needs a path (+0x2C) and a current skill (0xAABA: unit),
// else 0. The path gets distance 5 (0xAB14: path, 5) and move type 2 (0xAAFC: path, 2), the unit as its target
// (0xAAF0: path, unit, 0) and is computed (0xAC70: path). When that finds nothing it is retried with move type 1
// (0xAAFC: path, 1; 0xAAF0; 0xAC70) and a second failure gives 0. On success the skill is flagged (0xA78A: skill,
// 1) and the result is 1.
#pragma once
#include <cstdint>

namespace gb3890 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    const uint32_t path = e.rd32(unit + 0x2C);
    if (!path) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    e.h(0xAB14, 0, 0, 0, 0, 0, path, 5);
    e.h(0xAAFC, 0, 0, 0, 0, 0, path, 2);
    e.h(0xAAF0, 0, 0, 0, 0, 0, path, unit, 0);
    if (!e.h(0xAC70, 0, 0, 0, 0, 0, path)) {
        e.h(0xAAFC, 0, 0, 0, 0, 0, path, 1);
        e.h(0xAAF0, 0, 0, 0, 0, 0, path, unit, 0);
        if (!e.h(0xAC70, 0, 0, 0, 0, 0, path)) return 0;
    }
    e.h(0xA78A, 0, 0, 0, 0, 0, sk, 1);
    return 1;
}
}
