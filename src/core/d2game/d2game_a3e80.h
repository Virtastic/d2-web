// Seams of the 1.13c D2Game+0xA3E80 port (src/core/d2game/d2game_a3e80.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA3E80Env {
    uintptr_t dt;                                                         // *sgptDataTables (module global 0xF829C is a pointer to that pointer)
    uint32_t (*fA754)(uintptr_t t, uintptr_t unit, uintptr_t zero);       // D2Game+0xA754 thunk (stdcall, 3 words)
    int32_t (*fB0980)(int32_t x, int32_t y);                              // D2Game+0xB0980 (stdcall, 2 words)
    int32_t (*getStat)(uintptr_t unit, uint32_t stat, uint32_t layer);    // D2Game+0xA42A thunk (stdcall, 3 words)
    int32_t (*fA68E)(uintptr_t unit, uint32_t state);                     // D2Game+0xA68E thunk (stdcall, 2 words)
    int32_t (*fA718)(uintptr_t unit);                                     // D2Game+0xA718 thunk (stdcall, 1 word)
    uintptr_t (*fA3AC)(uintptr_t unit);                                   // D2Game+0xA3AC thunk (stdcall, 1 word)
    uintptr_t (*fA3B2)(uintptr_t unit);                                   // D2Game+0xA3B2 thunk (stdcall, 1 word)
    int32_t (*fA400)(uintptr_t unit, uintptr_t item, uintptr_t one);      // D2Game+0xA400 thunk (stdcall, 3 words)
    int32_t (*fA49C)(uintptr_t item, uint32_t line);                      // D2Game+0xA49C thunk (stdcall: item, __FILE__ at 0xFB6B0, line)
    uintptr_t (*fAD78)(uintptr_t t, uintptr_t unit, uintptr_t four);      // D2Game+0xAD78 thunk (stdcall, 3 words)
};
// eax = other (+0xC, +0x10 = x, y), ecx = unit, stack t (ret 4); returns the score (min 1), 0 for a low-rank monster class
extern "C" uint32_t d2_game_A3E80(uintptr_t other, uintptr_t unit, uintptr_t t, const D2GameA3E80Env* e);
