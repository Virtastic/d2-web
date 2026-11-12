// 1.13c D2Game+0x4C1C0: skill start callback "jab" (table entry). ecx as entered, edx = the unit, stack: the
// skill id and one more word (ret 8). dt = the data tables. An id outside the skill table (count at dt+0xBA0,
// rows of 0x23C bytes at dt+0xB98) or a null row: 0. Otherwise the unit's pending state is cleared (0xE07A0:
// eax = unit, ecx as entered) and the path's target (0xA598: the path at unit +0x2C) is looked up: 1 when there
// is a target other than the unit itself, else 0.
#pragma once
#include <cstdint>

namespace g4c1c0 {
template <class E> uint32_t Start(E& e, uint32_t dt, uint32_t ecx, uint32_t unit, uint32_t skill, uint32_t /*lvl*/)
{
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    if (!(e.rd32(dt + 0xB98) + skill * 0x23C)) return 0;
    e.h(0xE07A0, unit, ecx);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    return (t != unit && t) ? 1 : 0;
}
}
