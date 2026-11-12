// 1.13c D2Game+0xE95E0: object operate callback (table entry). ecx = the operate block (+0x04 the object),
// edx passes through. An object in mode 2 (+0x10) warps the player through its room tiles (0xC8950: eax = the
// object, ebx = the block). Result 0 either way.
#pragma once
#include <cstdint>

namespace ge95e0 {
template <class E> uint32_t Warp(E& e, uint32_t op, uint32_t edx)
{
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10) == 2) e.h(0xC8950, obj, op, edx, op);
    return 0;
}
}
