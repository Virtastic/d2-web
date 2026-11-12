// 1.13c D2Game: the tail shared by the unit mode change callbacks at 0x78150, 0x781C0, 0x78840 and 0x787C0. The
// animation is reset for the unit (0xDFF10: esi = unit), the mode change is announced (0xE0750: eax = game,
// ebx = unit) and the unit's frame events are rescheduled (0xDF7B0: the game on the stack, esi = unit), whose
// result is returned. The head is likewise shared: the unit leaves its pending event (0xDA750: the game on the
// stack, edi = unit) and takes the new mode (0xA4A2: unit, mode).
#pragma once
#include <cstdint>

namespace d2gmodeseq {
template <class E> void Head(E& e, uint32_t game, uint32_t unit, uint32_t mode)
{
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.j(0xDA750, unit);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, mode);
}
template <class E> uint32_t Tail(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xDFF10, 0, 0, 0, 0, unit);
    e.h(0xE0750, game, 0, 0, unit);
    return e.h(0xDF7B0, 0, 0, 0, 0, unit, game);
}
}
