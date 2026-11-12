// 1.13c D2Game+0xB3930: skill start callback of Andariel's spray (table entry). ecx unread, edx = the unit, two
// stack words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit) and read from its path (0xA598:
// path at +0x2C); none, or the unit itself: 0. The unit needs a current skill (0xAABA: unit), else 0. The
// target's x (d2game_unitxy.h) becomes the skill's first parameter (0xAC94: skill, x) and its y the second
// (0xACDC: skill, y). The result is 1.
#pragma once
#include <cstdint>
#include "d2game_unitxy.h"

namespace gb3930 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, D2GameUnitX(e, t));
    e.h(0xACDC, 0, 0, 0, 0, 0, sk, D2GameUnitY(e, t));
    return 1;
}
}
