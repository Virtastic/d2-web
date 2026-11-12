// 1.13c D2Game+0x98390: skill start callback (table entry). edx = the unit, two stack words unread (ret 8). The
// unit's inventory (+0x60) must exist, and its item of kind 0 must be found (0xAF64: inventory, 0). Result 1 when
// found, else 0.
#pragma once
#include <cstdint>

namespace g98390 {
template <class E> uint32_t NeedsItem(E& e, uint32_t unit)
{
    const uint32_t inv = e.rd32(unit + 0x60);
    if (!inv) return 0;
    return e.h(0xAF64, 0, 0, 0, 0, 0, inv, 0) != 0;
}
}
