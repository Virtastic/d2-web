// 1.13c D2Game+0x69AD0: object operate callback "toggle" (table entry). ecx = the operate block (+0x04 the object).
// An object in mode 0 (+0x10; a missing object counts as mode 0) goes to mode 1, one in mode 1 goes to mode 0
// (0xA4A2: unit, mode); any other mode is left alone. Result 0.
#pragma once
#include <cstdint>

namespace g69ad0 {
template <class E> uint32_t Toggle(E& e, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 4);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 0) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    else if (mode == 1) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 0);
    return 0;
}
}
