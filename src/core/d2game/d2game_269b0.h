// 1.13c D2Game+0x269B0: unit event callback (table entry). ecx = game, edx = unit; its two stack words are
// overwritten with (0, 2) and the unique-modifier dispatcher (0x268E0, same registers) takes over; its result is the
// result.
#pragma once
#include <cstdint>

namespace g269b0 {
template <class E> uint32_t RunModCallbacks(E& e, uint32_t game, uint32_t unit)
{
    return e.h(0x268E0, 0, game, unit, 0, 0, 0, 2);
}
}
