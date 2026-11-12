// 1.13c D2Game+0xF0E80 and +0xF0EE0: two player relation callbacks (table entries) that differ only in the
// relation bit (4 and 2). ecx = the game, edx = player A, three stack words (ret 0xC): player B, the wanted
// state, one unread. Both directions are prepared (0xF02F0: eax and ecx = the pair, the game on the stack), the
// current state of the bit is read (0xF02A0: the bit on the stack, esi = B, edi = A) and, when it differs from
// the wanted state, set (0xF0640: eax = the bit, ecx = A, esi = B; stack: the game, the state). Both players are
// then refreshed toward each other (0x8D120: eax and ebx = the pair); the second refresh is the result.
#pragma once
#include <cstdint>

namespace gf0e80 {
template <class E> uint32_t Relate(E& e, uint32_t game, uint32_t a, uint32_t b, uint32_t want, uint32_t bit)
{
    e.h(0xF02F0, b, a, 0, 0, 0, game);
    e.h(0xF02F0, a, b, 0, 0, 0, game);
    const uint32_t cur = e.h(0xF02A0, 0, 0, 0, 0, b, bit);
    e.j(0xF02A0, a);
    if (cur != want) e.h(0xF0640, bit, a, 0, 0, b, game, want);
    e.h(0x8D120, b, 0, 0, a);
    return e.h(0x8D120, a, 0, 0, b);
}
}
