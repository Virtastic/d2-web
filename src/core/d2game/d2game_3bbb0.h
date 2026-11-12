// 1.13c D2Game+0x3BBB0: missile hit callback "Baal effect control" (table entry). ecx = game, edx = the missile,
// one stack word (ret 4). A non-zero stack word, or a missile that fails the first test (0xB24C: missile, non-zero)
// ends at once. Otherwise the missile is flagged (0xA50E: missile, 1), its room is read (0xA478: missile), and
// when the quest check passes (0x33970: game, 0x24) Tyrael is spawned (0x5C1F0: ecx = missile; stack: game, room).
// The room is then refreshed (0xA51A: room, 1). The result is always 0.
#pragma once
#include <cstdint>

namespace g3bbb0 {
template <class E> uint32_t Hit(E& e, uint32_t game, uint32_t missile, uint32_t a)
{
    if (a) return 0;
    if (e.h(0xB24C, 0, 0, 0, 0, 0, missile)) return 0;
    e.h(0xA50E, 0, 0, 0, 0, 0, missile, 1);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, missile);
    if (e.h(0x33970, 0, 0, 0, 0, 0, game, 0x24)) e.h(0x5C1F0, 0, missile, 0, 0, 0, game, room);
    e.h(0xA51A, 0, 0, 0, 0, 0, room, 1);
    return 0;
}
}
