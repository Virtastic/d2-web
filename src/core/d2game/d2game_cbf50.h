// Seams of src/core/d2game/d2game_cbf50.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCBF50Env {
    uint32_t (*o10920)(uint32_t unit);                                   // thunk 0xA4C0: 1 stack word = unit, returns the player data
    uint32_t (*f_cb690)(uint32_t eax, uint32_t ebx, uint32_t s0, uint32_t s1);   // 0xCB690: EAX = unit, EBX = range, stack = x, y (ret 8)
    uint32_t (*f_1130)(uint32_t ecx);                                   // 0x1130: ECX = unit
    uint32_t (*f_1100)(uint32_t ecx);                                   // 0x1100: ECX = unit
    void     (*f_8bf80)(uint32_t eax, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);   // 0x8BF80: AL, EDX = id, 4 stack words (CL = 0)
};

uint32_t d2game_cbf50_port(uint32_t pkt, uint32_t unit, uint32_t game, uint32_t len, uint32_t outX, uint32_t outY,
                           const D2GameCBF50Env* e);
