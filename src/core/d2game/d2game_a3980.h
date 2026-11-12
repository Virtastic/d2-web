// 1.13c D2Game+0xA3980: AI state initialiser (table entry). One stack argument, the AI tick block; its first word
// is the AI control record, whose first parameter (+0x14) is cleared. EAX leaves holding the argument (ret 4).
#pragma once
#include <cstdint>

namespace ga3980 {
template <class E> uint32_t Init(E& e, uint32_t tick)
{
    e.wr32(e.rd32(tick) + 0x14, 0);
    return tick;
}
}
