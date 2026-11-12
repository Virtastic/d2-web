// 1.13c D2Game+0xA2EA0, transcribed from the 1.13c disassembly: the Unsummon skill server do-function.
// Convention: ECX = game, EDX = unit, stack skill id, skill level (ret 8). 0xAABA (D2Common 10511: unit) gives the unit's pet
// record; none returns 0. Otherwise 0xAD12 (D2Common 10980: record) and 0x97B50 (ECX = game, EDX = unit, stack that value
// and 1; ret 8): the 1 is pushed for 10980 but that callee pops one word, so it stays on the stack as 0x97B50's second word.
#pragma once
#include <cstdint>

namespace ra2ea0 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    (void)skill; (void)level;
    const uint32_t rec = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!rec) return 0;
    const uint32_t v = e.h(0xAD12, 0, 0, 0, 0, 0, rec);
    e.h(0x97B50, 0, game, unit, 0, 0, v, 1);
    return 1;
}
}
