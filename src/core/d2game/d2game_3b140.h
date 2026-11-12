// 1.13c D2Game+0x3B140: monster skill state callback (table entry). ecx = game, edx = the unit, one stack word
// (ret 4). A unit whose room (0xA478) tests set at 0xA808 gives 1. The unit's flags come from 0xB24C; with a
// zero stack word, bit 2 of them, or bit 2 of the path value (0xAF2E: unit +0x2C), gives 1. With bit 0: a zero
// stack word gives 3; otherwise 0xE07A0 runs (eax = unit, ecx = game) and the path's target (0xA598, taken as
// none when it is the unit itself) is compared with the stack word: equal 3, else 4. With bit 1 and not bit 0:
// a positive 0xB102 value gives 3 with bit 2 and 4 without; otherwise 0x3B070 (eax = unit; game) decides,
// nonzero 1 and zero 4. Neither bit: 3.
#pragma once
#include <cstdint>

namespace g3b140 {
template <class E> uint32_t State(E& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    if (e.h(0xA808, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit))) return 1;
    const uint32_t f = e.h(0xB24C, 0, 0, 0, 0, 0, unit);
    if (!s0 && (f & 4)) return 1;
    const uint32_t g = e.h(0xAF2E, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (!s0 && (g & 4)) return 1;
    if (f & 1) {
        if (!s0) return 3;
        e.h(0xE07A0, unit, game);
        uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
        if (t == unit) t = 0;
        return s0 == t ? 3 : 4;
    }
    if (f & 2) {
        if ((int32_t)e.h(0xB102, 0, 0, 0, 0, 0, unit) > 0) return (f & 4) ? 3 : 4;
        return e.h(0x3B070, unit, 0, 0, 0, 0, game) ? 1 : 4;
    }
    return 3;
}
}
