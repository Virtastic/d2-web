// Seams of the 1.13c D2Game+0xA4B90 port (src/core/d2game/d2game_a4b90.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA4B90Env {
    int32_t (*o10750)(uintptr_t pData);                      // D2Common 10750 via D2Game+0xA3B8 (stdcall, 1 word)
    int32_t (*o10867)(uintptr_t pData);                      // D2Common 10867 via D2Game+0xA454 (stdcall, 1 word)
    void (*o11103)(uintptr_t unit, uintptr_t zero);          // D2Common 11103 via D2Game+0xA496 (stdcall, 2 words)
    uintptr_t (*f21320)(uint32_t classId);                   // D2Game+0x21320: class record, EAX = unit+4
    const uint32_t* masks;                                   // gdwBitMasks (module global 0xF8BE0 holds this pointer)
    void (*f95E10)(uintptr_t game, const uint32_t* blk, uintptr_t one);   // D2Game+0x75E10: send packet block
};
// ecx = game, edx = unit, stack ctx (a pointer to the pointer to the AI state; ret 4)
extern "C" void d2_game_A4B90(uintptr_t game, uintptr_t unit, uintptr_t ctx, const D2GameA4B90Env* e);
