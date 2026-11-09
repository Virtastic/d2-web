// D2Client+0xAEFB0 env seam (packet handler: two unit lookups, then the skill/aura-style apply through 0xA2950 or 0xA1B10).
#pragma once
#include <cstdint>
struct D2cAEFB0Env {
    uintptr_t (*lookup)(uint32_t id, uintptr_t row, uint32_t type);   // D2Client+0xA4E20 (EAX = id & 0x7F, ECX = id, EDX = row, stack type)
    uintptr_t unitTable;                                              // 0x6FBBA608 (row = unitTable + type * 0x80 pointers)
    const uintptr_t* sgptDataTables;                                  // the variable the IAT slot at 0x6FB7E474 points to
    const uint8_t* gdwBitMasks;                                       // the array the IAT slot at 0x6FB7EE50 points to
    // EAX = u1, six stack words (u2, index, w4, d10, d14, b1)
    void (*f52950)(uintptr_t u1, uintptr_t u2, int32_t i, int32_t w4, uint32_t d10, uint32_t d14, uint32_t b1);   // D2Client+0xA2950
    void (*f51B10)(uintptr_t u1, uintptr_t u2, int32_t i, int32_t w4, uint32_t d10, uint32_t d14, uint32_t b1);   // D2Client+0xA1B10
};
extern "C" void d2c_aefb0(const D2cAEFB0Env* e, const uint8_t* pkt);
