// 1.13c D2Game+0x2E760: object init callback (table entry). ecx = the init block: +0x04 the object unit, +0x10 the
// objects.txt record. The byte at record+0x180, doubled (8-bit), goes to byte +4 of the object's data record
// (unit+0x14). EAX leaves holding the unit.
#pragma once
#include <cstdint>

namespace g2e760 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t v = e.rd8(e.rd32(op + 0x10) + 0x180);
    const uint32_t unit = e.rd32(op + 4);
    e.wr8(e.rd32(unit + 0x14) + 4, (v << 1) & 0xFFu);
    return unit;
}
}
