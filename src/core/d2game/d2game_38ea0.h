// 1.13c D2Game+0x38EA0: missile hit callback (table entry). edx = the missile, one stack word = the unit hit
// (ret 4). On a wall hit (no unit) the missile's room (0xA478) is looked up and, when there is one, flagged
// (0xA51A: room, 1). Result 0.
#pragma once
#include <cstdint>

namespace g38ea0 {
template <class E> uint32_t FlagRoom(E& e, uint32_t missile, uint32_t target)
{
    if (!target) {
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, missile);
        if (room) e.h(0xA51A, 0, 0, 0, 0, 0, room, 1);
    }
    return 0;
}
}
