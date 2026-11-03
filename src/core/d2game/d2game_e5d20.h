// 1.13c D2Game+0xE5D20 : quest-state gate, category 0x15. fastcall ecx = game, edx = unit.
// The body is a template over its three callees so the engine (weak Ord10920/Ord10174/D2GAME_C554F0 in d2game_batch_b65.cpp)
// and the FN probe (src/core/fn/d2game_e5d20.cpp, recorder hooks) run the same code.
#pragma once
#include <cstdint>

template <class Ord10920, class Ord10174, class Act354F0>
static inline intptr_t d2game_e5d20_body(intptr_t game, void* unit, Ord10920 o10920, Ord10174 o10174, Act354F0 act)
{
    intptr_t r = o10920(unit);
    intptr_t v = (intptr_t)*(uint32_t*)(r + 0x10 + *(uint8_t*)(game + 0x6d) * 4);
    if (((o10174(v, 0x15, 0) != 0 || o10174(v, 0x15, 0xf) != 0) &&
         o10174(v, 0x15, 0xd) == 0) && o10174(v, 0x15, 0xe) == 0)
        return 0;
    act(unit, game);
    return 0;
}
