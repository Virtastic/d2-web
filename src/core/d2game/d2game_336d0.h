// 1.13c D2Game+0x336D0: object init callback (table entry). ecx = the init block (+0x00 game, +0x04 the object).
// Queues the object's timer event (0x8EA80: ecx = game, edx = object, stack = event 3, game frame (game+0xA8) + 25,
// 0, 0, 0); its result is left in EAX.
#pragma once
#include <cstdint>

namespace g336d0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t w4[1] = { 0 };
    const uint32_t r = e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 3, e.rd32(game + 0xA8) + 0x19, 0, 0);
    e.x(0x8EA80, w4, 1);
    return r;
}
}
