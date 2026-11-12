// 1.13c D2Game+0xA5D40: monster think callback (table entry). ecx = game, edx = the monster, one stack word
// unread (ret 4). A monster with a room (0xA478) is told to idle for 200 frames (0xF22C0: eax = game, esi = monster,
// one stack word = 200) and that call's result is the result; with no room the result is 0.
#pragma once
#include <cstdint>

namespace ga5d40 {
template <class E> uint32_t Idle(E& e, uint32_t game, uint32_t unit)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    return e.h(0xF22C0, game, 0, 0, 0, unit, 0xC8);
}
}
