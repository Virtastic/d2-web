// Seam for D2Game+0x5A820 ACT5Q6_RollEligibleRow.
#pragma once
#include <cstdint>
struct D2Game5A820Env {
    void (*dfcc0)(uint32_t game);   // 0xDFCC0: EAX = 0, EDI = game, EBX = callback 0x599D0, one stack word = 0
    void (*fatal)();                // the missing-RNG error path (D2Common #10265/#10024, _exit(-1))
};
// p1 = ECX (its first dword is the game), dt = *sgptDataTables, bitMasks = gdwBitMasks.
void d2game_port_5a820(uint32_t p1, uint32_t dt, uint32_t bitMasks, const D2Game5A820Env* env);
