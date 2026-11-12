// 1.13c D2Game+0x59A60: object operate callback "ancient statue, third sound" (table entry). ecx = the operate
// block ([8] the object). The object's event word (+0x6E) becomes 0x13 and its event source (+0x70) the object
// itself, the room is told to refresh it (0xA4DE: object) and flag 0x400 is set in the flag word (+0xC4). The
// result is 0. A missing object ends in the assert-and-exit stub, which is not modelled.
#pragma once
#include <cstdint>

namespace g59a60 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 8);
    e.wr8(obj + 0x6E, 0x13);
    e.wr8(obj + 0x6F, 0);
    e.wr32(obj + 0x70, obj);
    e.h(0xA4DE, 0, 0, 0, 0, 0, obj);
    e.wr32(obj + 0xC4, e.rd32(obj + 0xC4) | 0x400);
    return 0;
}
}
