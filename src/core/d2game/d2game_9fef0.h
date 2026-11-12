// 1.13c D2Game+0x9FEF0: skill start callback of unsummon (table entry). ecx unread, edx = the unit, two stack
// words unread (ret 8). The unit's target is resolved (0xE07A0: eax = unit) and read from its path (0xA598: path
// at +0x2C). The target must exist, differ from the unit and be a monster (type 1), the unit must be a player
// (type 0) and the target's owner (0x21260: ecx = target) must be the unit, else 0. The pet type of the target is
// found (0x96600: eax = unit, ebx = the target's id at +0x0C) and its record fetched (0x1B60: eax = that type);
// no record: 0. The record's byte +0x04 must share a bit with byte +0x0C of the block the module pointer at
// 0xF8BE0 names, and the unit must have a current skill (0xAABA: unit), else 0. The skill's first parameter is
// set to the target's id (0xAC94: skill, id) and the result is 1.
#pragma once
#include <cstdint>

namespace g9fef0 {
template <class E> uint32_t Start(E& e, uint32_t bm, uint32_t unit)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (t == unit || !t || e.rd32(t) != 1 || e.rd32(unit) != 0) return 0;
    if (e.h(0x21260, 0, t) != unit) return 0;
    const uint32_t id = e.rd32(t + 0xC);
    const uint32_t type = e.h(0x96600, unit, 0, 0, id);
    const uint32_t rec = e.h(0x1B60, type);
    if (!rec) return 0;
    if (!(e.rd8(bm + 0xC) & e.rd8(rec + 4))) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, e.rd32(t + 0xC));
    return 1;
}
}
