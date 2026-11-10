// Seams of src/core/d2game/d2game_ce150.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCE150Env {
    const uint8_t* tab;                                            // module data 0x102CE4 (DAT_6fd22ce4): 12-byte rows, +0 packet byte, +4 0x8C230 byte
    uint32_t (*f_e03a0)(uint32_t eax, uint32_t ecx, uint32_t edx); // 0xE03A0: EAX = client+0x16c, ECX = client+0x1a8, EDX = client+0x170
    void (*f_e07a0)(uint32_t eax, uint32_t ecx);                   // 0xE07A0: EAX = unit, ECX = game
    uint32_t (*o10392)(uint32_t path);                             // thunk 0xA598 (stdcall 1)
    uint32_t (*o10331)(uint32_t unit);                             // thunk 0xA478 (stdcall 1): the room
    uint32_t (*f_1130)(uint32_t ecx);                              // 0x1130: ECX = unit, y
    uint32_t (*f_1100)(uint32_t ecx);                              // 0x1100: ECX = unit, x
    void (*f_8c230)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    uint32_t (*f_a3b8)(uint32_t path);                             // thunk 0xA3B8 (stdcall 1): packet word +14
    uint32_t (*f_a454)(uint32_t path);                            // thunk 0xA454 (stdcall 1): packet word +12
    uint32_t (*f_aad8)(uint32_t path);                             // thunk 0xAAD8 (stdcall 1): packet word +9
    uint32_t (*f_aac6)(uint32_t path);                             // thunk 0xAAC6 (stdcall 1): packet word +7
    void (*f_8a3e0)(uint32_t eax, const uint8_t* pkt, uint32_t len); // 0x8A3E0: EAX = client, stack = packet, 0x10
};

// ECX = game, EDX = unit, stack = client, table index; ret 8 (void).
void d2game_ce150_port(uint32_t game, uint32_t unit, uint32_t client, uint32_t idx, const D2GameCE150Env* e);
