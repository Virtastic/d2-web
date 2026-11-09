// 1.13c D2Game+0x5B8C0 ACT5Q3_StatusFilter: ECX = quest record, EDX = player unit; stack (unused, quest flags, byte out) (ret 0xC); returns AL = 1.
// Clears *out, then reads the player's quest flags through Ordinal 10174 (thunk 0xA568): flag (0x25, 0) set leaves it 0; flags 1 or 0xD
// pick the reward branch keyed on quest+0xE0 (bits 8/9: 6, 5 or 0xD); otherwise, with quest+9 set, 0x5B030 (EAX = unit, stack quest) gives 4,
// flag 0xE gives 0xC, and quest+0xC below 5 copies quest+0xB.
#pragma once
#include <cstdint>

namespace d2g5b8c0 {
template <class E> uint32_t Run(E& e, uint32_t q, uint32_t unit, uint32_t flags, uint32_t out)
{
    auto F = [&](uint32_t a, uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, flags, a, b); };
    e.wr8(out, 0);
    if (F(0x25, 0)) return 1;
    if (!F(0x25, 1) && !F(0x25, 0xD)) {
        if (!e.rd8(q + 9)) return 1;
        if (e.h(0x5B030, unit, 0, 0, 0, 0, q)) { e.wr8(out, 4); return 1; }
        if (F(0x25, 0xE)) { e.wr8(out, 0xC); return 1; }
        if (e.rd8(q + 0xC) < 5) e.wr8(out, e.rd8(q + 0xB));
        return 1;
    }
    if (F(e.rd32(q + 0xE0), 8) && !F(e.rd32(q + 0xE0), 9)) { e.wr8(out, 6); return 1; }
    e.wr8(out, F(e.rd32(q + 0xE0), 8) ? 0xD : 5);
    return 1;
}
}
