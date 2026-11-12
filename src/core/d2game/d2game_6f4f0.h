// 1.13c D2Game+0x6F4F0: object operate callback (table entry). ecx = the operate block (+0x04 the object),
// edx passes through. An object whose mode word (+0x10) has low half 2 hands over to the stair step (0x6DFC0: eax =
// the mode word, same ecx and edx) and its result is the result; anything else gives 0.
#pragma once
#include <cstdint>

namespace g6f4f0 {
template <class E> uint32_t Stair(E& e, uint32_t op, uint32_t edx)
{
    const uint32_t obj = e.rd32(op + 4);
    if (!obj) return 0;
    const uint32_t mode = e.rd32(obj + 0x10);
    if ((mode & 0xFFFFu) != 2) return 0;
    return e.h(0x6DFC0, mode, op, edx);
}
}
