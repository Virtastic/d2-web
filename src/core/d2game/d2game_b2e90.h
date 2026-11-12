// 1.13c D2Game+0xB2E90, transcribed from the 1.13c disassembly: the Submerge skill server do-function.
// Convention: ECX = game (unused), EDX = unit, stack skill id, skill level (ret 8). The unit's frame pair is read through 0xACA0
// (D2Common 10569: unit, &a, &b; ret 0xc); the unit's byte +0x4E is cleared, and +0x3C is cleared when a <= 0. Always returns 1.
#pragma once
#include <cstdint>

namespace rb2e90 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    (void)game; (void)skill; (void)level;
    uint32_t ab[2] = { 0, 0 };
    e.h(0xACA0, 0, 0, 0, 0, 0, unit, e.stk(&ab[0]), e.stk(&ab[1]));
    e.wr8(unit + 0x4E, 0);
    if ((int32_t)ab[0] <= 0) e.wr32(unit + 0x3C, 0);
    return 1;
}
}
