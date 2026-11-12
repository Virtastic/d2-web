// 1.13c D2Game+0x38E70: missile damage callback (table entry). ecx and edx unread; the second stack argument is the
// damage record: its cold length (+0x30) moves to the freeze length (+0x34) and the cold length is cleared.
// EAX leaves holding the record (ret 8).
#pragma once
#include <cstdint>

namespace g38e70 {
template <class E> uint32_t ColdToFreeze(E& e, uint32_t dmg)
{
    e.wr32(dmg + 0x34, e.rd32(dmg + 0x30));
    e.wr32(dmg + 0x30, 0);
    return dmg;
}
}
