// 1.13c D2Game+0x74E40: monster event callback (table entry). edx = unit, two stack words unread (ret 8). For a
// monster (type 1) with a data record (+0x14) the two target-override words (+0x38, +0x34) are cleared.
// EAX: untouched for a null unit, the unit type for a non-monster, else the data record pointer (0 when absent).
#pragma once
#include <cstdint>

namespace g74e40 {
template <class E> uint32_t ClearOverride(E& e, uint32_t eaxIn, uint32_t unit)
{
    if (!unit) return eaxIn;
    const uint32_t type = e.rd32(unit);
    if (type != 1) return type;
    const uint32_t data = e.rd32(unit + 0x14);
    if (!data) return 0;
    e.wr32(data + 0x38, 0);
    e.wr32(data + 0x34, 0);
    return data;
}
}
