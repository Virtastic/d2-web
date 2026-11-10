// Seams of src/core/d2game/d2game_cdf30.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCDF30Env {
    uint32_t (*o10747)(uint32_t unit);                     // thunk 0xA856 (stdcall 1)
    uint32_t (*f_cb1f0)(uint32_t ebx, uint32_t s0);        // 0xCB1F0: EBX = unit, 1 stack word = skill (ret 4)
    uint32_t (*o10787)(uint32_t unit, uint32_t skill);     // thunk 0xA8C8 (stdcall 2): the use mode
    uint32_t (*o10858)(uint32_t ecx);                      // thunk 0xA928: ECX = skill, the skills.txt record
    uint32_t (*o10708)(uint32_t ecx, uint32_t edx, uint32_t s0);   // thunk 0xA3D0: ECX = unit, EDX = 0, 1 stack word = -1
    uint32_t (*o10952)(uint32_t unit, uint32_t skill);     // thunk 0xAB68 (stdcall 2)
    void (*f_cbed0)(uint32_t eax);                         // 0xCBED0: EAX = unit
    void (*f_78550)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);   // ret 0x14
};

uint32_t d2game_cdf30_port(uint32_t skill, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, const D2GameCDF30Env* e);
