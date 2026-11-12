// 1.13c D2Game+0x27270: skill start callback "target revivable corpse" (table entry). edx = the unit, two stack
// words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit), then read from its path (0xA598: path
// at +0x2C). No target, or the unit itself: 0. Otherwise the corpse test (0xB342: target) must be non-zero and the
// target must not carry state 0x76 (0xA68E: target, 0x76); then the corpse test's value is the result, else 0.
#pragma once
#include <cstdint>

namespace g27270 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    const uint32_t r = e.h(0xB342, 0, 0, 0, 0, 0, t);
    if (!r) return 0;
    return e.h(0xA68E, 0, 0, 0, 0, 0, t, 0x76) ? 0 : r;
}
}
