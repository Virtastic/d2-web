// Seams of the 1.13c D2Game+0xA4610 port (src/core/d2game/d2game_a4610.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA4610Env {
    void (*fail)(uint32_t line);                                        // Fog 10265(line) + 10024 + _exit(-1): the two asserts (0x37A null callback, 0x395 bad target node); never returns
    void (*fA478)(uintptr_t unit);                                      // D2Game+0xA478 thunk (stdcall, 1 word; result unused)
    int32_t (*fA454)(uintptr_t path);                                   // D2Game+0xA454 thunk: x of a path (stdcall, 1 word)
    int32_t (*fA3B8)(uintptr_t path);                                   // D2Game+0xA3B8 thunk: y of a path (stdcall, 1 word)
    int32_t (*cull)(uintptr_t unit, uintptr_t cand);                    // pfCull, __fastcall: ECX = unit, EDX = candidate
    int32_t (*f3E80)(uintptr_t aiCmd, uintptr_t cand, uintptr_t unit);  // D2Game+0xA3E80: EAX = aiCmd, ECX = candidate, stack unit (ret 4)
    int32_t (*fB08F0)(uintptr_t unit, int32_t x, int32_t y);            // D2Game+0xB08F0: EAX = unit, stack x, y (ret 8)
    uint32_t (*fA754)(uintptr_t unit, uintptr_t target, uintptr_t z);   // D2Game+0xA754 thunk (stdcall, 3 words)
    void (*fA5FE)(uintptr_t path, uintptr_t target);                    // D2Game+0xA5FE thunk (stdcall, 2 words)
    void (*fAAFC)(uintptr_t path, uint32_t type);                       // D2Game+0xAAFC thunk (stdcall, 2 words)
    void (*fAAF0)(uintptr_t path, uintptr_t unit, uintptr_t z);         // D2Game+0xAAF0 thunk (stdcall, 3 words)
    int32_t (*fAC70)(uintptr_t path);                                   // D2Game+0xAC70 thunk (stdcall, 1 word)
};
// stdcall, ret 0x18: game, unit, *a2 := best score, *a3 := candidates scored, aiCmd, pfCull. Returns the best target unit (or 0).
extern "C" uintptr_t d2_game_A4610(uintptr_t game, uintptr_t unit, int32_t* a2, int32_t* a3, uintptr_t aiCmd, uintptr_t pfCull, const D2GameA4610Env* e);
