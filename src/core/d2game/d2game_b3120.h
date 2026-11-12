// 1.13c D2Game+0xB3120: skill start callback of the maggot's burrow-down (table entry). ecx unread, edx = the
// unit, two stack words unread (ret 8). Bits 1-3 of the unit's flags at +0xC4 are cleared. The unit's y and then
// x are read (d2game_unitxy.h), its room fetched (0xA478: unit) and the collision mask 0x1000 applied there
// (0xAD30: room, x, y, 5, 0x1000). The result is 1.
#pragma once
#include <cstdint>
#include "d2game_unitxy.h"

namespace gb3120 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFF1u);
    const uint32_t y = D2GameUnitY(e, unit);
    const uint32_t x = D2GameUnitX(e, unit);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    e.h(0xAD30, 0, 0, 0, 0, 0, room, x, y, 5);
    const uint32_t w[1] = { 0x1000 };
    e.x(0xAD30, w, 1);
    return 1;
}
}
