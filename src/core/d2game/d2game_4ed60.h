// 1.13c D2Game+0x4ED60: skill start callback "target is a revivable corpse" (table entry). ecx unread, edx = the
// unit, two stack words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit) and read from its
// path (0xA598: path at +0x2C). The target must exist, differ from the unit and be a monster (type 1); its class
// must pass the flag test (0x13B0: eax = class at +0x04, edx = 8), it must pass the corpse test (0xDFA90: eax =
// target) and the state test (0xADC0: target, 0); any failure: 0. The result is 1 when the last test (0x9F120:
// eax = target; stack: 7) is non-zero, else 0.
#pragma once
#include <cstdint>

namespace g4ed60 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t || e.rd32(t) != 1) return 0;
    if (!e.h(0x13B0, e.rd32(t + 4), 0, 8)) return 0;
    if (!e.h(0xDFA90, t)) return 0;
    if (!e.h(0xADC0, 0, 0, 0, 0, 0, t, 0)) return 0;
    return e.h(0x9F120, t, 0, 0, 0, 0, 7) ? 1 : 0;
}
}
