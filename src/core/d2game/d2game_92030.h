// 1.13c D2Game+0x92030: skill server-start callback (table entry). edx = the attacker, two stack words unread (ret
// 8). The attacker's skill state is prepared (0xE07A0: eax and esi = the attacker, ecx and edx as entered), then
// the target of its path (+0x2C) is fetched (0xA598). No target, or the attacker itself: 0. Otherwise the range
// check between the two (0xA754: attacker, target, 0) is the result.
#pragma once
#include <cstdint>

namespace g92030 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xE07A0, unit, game, unit, 0, unit);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (target == unit || !target) return 0;
    return e.h(0xA754, 0, 0, 0, 0, 0, unit, target, 0);
}
}
