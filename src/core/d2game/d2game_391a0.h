// 1.13c D2Game+0x391A0: monster skill check (table entry). ecx unread, edx = the unit, one stack word (ret 4).
// No monster record: 1. With a nonzero stack word the unit's two values (0xB28E, 0xAD06: unit) are compared:
// the first below the second less the record's word at +0x4C (signed) gives 2. Otherwise 0.
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g391a0 {
template <class E> uint32_t Check(E& e, uint32_t dt, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMonRec(e, dt, unit);
    if (!rec) return 1;
    if (s0) {
        const int32_t a = (int32_t)e.h(0xB28E, 0, 0, 0, 0, 0, unit);
        const int32_t b = (int32_t)e.h(0xAD06, 0, 0, 0, 0, 0, unit);
        if (a < b - (int32_t)e.rd32(rec + 0x4C)) return 2;
    }
    return 0;
}
}
