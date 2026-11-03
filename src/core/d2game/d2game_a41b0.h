// Seams of the 1.13c D2Game+0xA41B0 port (src/core/d2game/d2game_a41b0.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA41B0Env {
    void (*o11103)(uintptr_t unit, uintptr_t zero);               // D2Common 11103 via D2Game+0xA496 (stdcall, 2 words)
    void (*o11005)(uintptr_t pData, uintptr_t one);               // D2Common 11005 via D2Game+0xAD18 (stdcall, 2 words)
    void (*f95E10)(uintptr_t game, const uint32_t* blk, uintptr_t one);   // D2Game+0x75E10: send packet block (stdcall, 3 words)
};
// eax = mode, edx (low byte) = kind, esi = unit, stack: game, target (ret 8)
extern "C" void d2_game_A41B0(int32_t mode, uint32_t edx, uintptr_t unit, uintptr_t game, uintptr_t target, const D2GameA41B0Env* e);
