// 1.13c D2Game+0xA39B0: AI state initialiser (table entry). ecx = game, one stack argument = the AI tick block whose
// first word is the AI control record: parameter 1 (+0x14) := the game frame (game+0xA8), parameter 2 (+0x18) := -1.
// EAX leaves holding the control record (ret 4).
#pragma once
#include <cstdint>

namespace ga39b0 {
template <class E> uint32_t Init(E& e, uint32_t game, uint32_t tick)
{
    const uint32_t ctl = e.rd32(tick);
    e.wr32(ctl + 0x14, e.rd32(game + 0xA8));
    e.wr32(ctl + 0x18, 0xFFFFFFFFu);
    return ctl;
}
}
