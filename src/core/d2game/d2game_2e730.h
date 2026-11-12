// 1.13c D2Game+0x2E730: object init callback (table entry). ecx = the init block; the object unit (+0x04) is put
// in animation mode 1 (0xA4A2: unit, mode), whose result is left in EAX.
#pragma once
#include <cstdint>

namespace g2e730 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
}
}
