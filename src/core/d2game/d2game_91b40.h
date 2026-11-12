// 1.13c D2Game+0x91B40: skill server-start callback (table entry). edx = the attacker, two stack words unread (ret
// 8). The attacker's skill state is prepared (0xE07A0: eax and esi = the attacker, ecx and edx as entered), then
// the target of its path (+0x2C) is fetched (0xA598). Result 1 when there is a target other than the attacker
// itself, else 0.
#pragma once
#include <cstdint>

namespace g91b40 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xE07A0, unit, game, unit, 0, unit);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    return (target == unit || !target) ? 0 : 1;
}
}
