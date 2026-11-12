// 1.13c D2Game+0x15530: AI state initialiser (table entry). One stack argument, the AI tick block; its first word
// is the AI control record, whose first parameter (+0x14) is set to -1. EAX leaves holding the argument (ret 4).
#pragma once
#include <cstdint>

namespace g15530 {
template <class E> uint32_t Init(E& e, uint32_t tick)
{
    e.wr32(e.rd32(tick) + 0x14, 0xFFFFFFFFu);
    return tick;
}
}
