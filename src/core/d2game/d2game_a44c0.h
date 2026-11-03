// Seams of the 1.13c D2Game+0xA44C0 port (src/core/d2game/d2game_a44c0.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA44C0Env {
    uintptr_t dt;                                                   // *sgptDataTables (module global 0xF829C is a pointer to that pointer)
    const uint32_t* masks;                                          // gdwBitMasks (module global 0xF8BE0 holds this pointer)
    uint32_t (*f3D60)(uint32_t flagEax, int32_t skill, uintptr_t unit);   // D2Game+0xA3D60: EAX = flag, ECX = skill, EDX = unit
    int32_t (*o10494)(uintptr_t unit, int32_t skill);               // D2Common 10494 via D2Game+0xA68E (stdcall, 2 words)
    uintptr_t (*o10871)(uintptr_t unit, int32_t skill);             // D2Common 10871 via D2Game+0xA5D4 (stdcall, 2 words)
    int32_t (*o10680)(uintptr_t p, int32_t skill, int32_t zero);    // D2Common 10680 via D2Game+0xA712 (stdcall, 3 words)
};
// eax = ctx (ctx[+8] = unit8, ctx[+0x18] = boss flag), ecx = skill, ebx = unit, stack: arg1 (unused), arg2 (EAX of 0xA3D60); ret 8
extern "C" uint32_t d2_game_A44C0(uintptr_t ctx, int32_t skill, uintptr_t unit, uint32_t arg2, const D2GameA44C0Env* e);
