// Seams of the 1.13c D2Game+0xA4A10 port (src/core/d2game/d2game_a4a10.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA4A10Env {
    uintptr_t dt;                                                           // *sgptDataTables (module global 0xF829C is a pointer to that pointer)
    uintptr_t (*fE03A0)(uintptr_t eax, uintptr_t edx, uintptr_t ecx);       // D2Game+0xE03A0: EAX = q[+0x30], EDX = q[+0x2C], ECX = q[+0x28]
    uintptr_t (*oA478)(uintptr_t p);                                        // D2Game+0xA478 thunk (stdcall, 1 word)
    uintptr_t (*oA808)(uintptr_t p);                                        // D2Game+0xA808 thunk (stdcall, 1 word)
    void (*o11103)(uintptr_t unit, uintptr_t zero);                         // D2Common 11103 via D2Game+0xA496 (stdcall, 2 words)
    void (*f95E10)(uintptr_t game, const uint32_t* blk, uintptr_t one);     // D2Game+0x75E10: send packet block
    void (*fF21E0)(uintptr_t eax, uintptr_t game, uintptr_t z0, uintptr_t z1);   // D2Game+0xF21E0: EAX = 0, stack game, 0, 0 (ret 0xC)
    uintptr_t (*oA3F4)(uintptr_t unit, int32_t skill);                      // D2Game+0xA3F4 thunk (fastcall: ECX = unit, EDX = skill)
    uintptr_t (*oA400)(uintptr_t unit, uintptr_t a, uintptr_t one);         // D2Game+0xA400 thunk (stdcall, 3 words)
    uint32_t (*oAC9A)(uintptr_t unit, uintptr_t v144, int32_t skill, uintptr_t a400);   // D2Game+0xAC9A thunk (stdcall, 4 words)
};
// eax = unit; stack: game, ctx (pointer to the pointer to the AI state), consume flag (ret 0xC)
extern "C" uint32_t d2_game_A4A10(uintptr_t unit, uintptr_t game, uintptr_t ctx, uint32_t consume, const D2GameA4A10Env* e);
