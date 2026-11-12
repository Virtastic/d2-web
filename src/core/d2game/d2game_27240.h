// 1.13c D2Game+0x27240: skill start callback "find item" (table entry). edx = the unit, two stack words unread
// (ret 8). The unit's target is resolved (0xE07A0: eax = unit), then read from its path (0xA598: path at +0x2C).
// No target, or the unit itself: 0. Otherwise the target is tested (0xB330: target) and the result is 1 when that
// test is non-zero, else 0.
#pragma once
#include <cstdint>

namespace g27240 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    return e.h(0xB330, 0, 0, 0, 0, 0, t) ? 1 : 0;
}
}
