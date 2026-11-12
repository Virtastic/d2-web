// 1.13c D2Game+0xA3990: AI state initialiser (table entry). One stack argument, the AI tick block; its first word
// is the AI control record: parameter 1 (+0x14) := -1, parameter 2 (+0x18) := 1, parameter 3 (+0x1C) := 0.
// EAX leaves holding the control record (ret 4).
#pragma once
#include <cstdint>

namespace ga3990 {
template <class E> uint32_t Init(E& e, uint32_t tick)
{
    const uint32_t ctl = e.rd32(tick);
    e.wr32(ctl + 0x14, 0xFFFFFFFFu);
    e.wr32(ctl + 0x18, 1);
    e.wr32(ctl + 0x1C, 0);
    return ctl;
}
}
