// 1.13c D2Game+0x4F920, transcribed from the 1.13c disassembly: the Poison Dagger skill server do-function.
// Convention: ECX = game, EDX = unit, stack skill id, skill level (ret 8). A non-null unit gets +0xC4 |= 0x40; then
// 0xE07A0 (EAX = unit, ECX = game), the target from 0xA598 (D2Common 10392: unit +0x2C). No target, or the unit itself,
// returns 0; else 0xDED30 (game, unit, target; ret 0xc) and 1.
#pragma once
#include <cstdint>

namespace r4f920 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    (void)skill; (void)level;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
    return 1;
}
}
