// 1.13c D2Game+0x4EF20: skill start callback "consumable corpse outside town" (table entry). edx = the unit, two
// stack words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit), then read from its path
// (0xA598: path at +0x2C). No target, the unit itself, or a target whose room (0xA478: target) is in town
// (0xA808: room, non-zero): 0. Otherwise the corpse-consumable test (0xADC0: target, 0) is the result.
#pragma once
#include <cstdint>

namespace g4ef20 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, t);
    if (e.h(0xA808, 0, 0, 0, 0, 0, room)) return 0;
    return e.h(0xADC0, 0, 0, 0, 0, 0, t, 0);
}
}
