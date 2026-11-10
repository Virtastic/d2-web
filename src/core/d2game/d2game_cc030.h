// Seams of src/core/d2game/d2game_cc030.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCC030Env {
    uint32_t (*o10920)(uint32_t unit);                       // thunk 0xA4C0: 1 stack word = unit, returns the player data
    uint32_t (*f_49240)(uint32_t eax);                       // 0x49240: EAX = client, returns the client's update record
    uint32_t (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);   // thunk 0xA42A: stat value (stdcall 3)
    uint32_t (*o10913)(uint32_t unit);                       // thunk 0xA3A0: max life (stdcall 1)
    uint32_t (*f_1100)(uint32_t ecx);                        // 0x1100: ECX = unit, x
    uint32_t (*f_1130)(uint32_t ecx);                        // 0x1130: ECX = unit, y
    uint32_t (*pathX)(uint32_t path);                        // thunk 0xAAC6 (stdcall 1)
    uint32_t (*pathY)(uint32_t path);                        // thunk 0xAAD8 (stdcall 1)
    uint32_t (*f_cb840)(uint32_t eax);                       // 0xCB840: EAX = unit, byte result
    uint32_t (*f_cb720)(uint32_t eax);                       // 0xCB720: EAX = unit, byte result
    void (*f_8b750)(uint32_t client, uint32_t st10, uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);   // 6 stack words
    void (*f_8b7f0)(uint32_t client, uint32_t hp, uint32_t mana, uint32_t st10, uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);
    void (*f_8b8c0)(uint32_t client, uint32_t hp, uint32_t mana, uint32_t st10, uint32_t b840, uint32_t b720,
                    uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);
    void (*f_8be10)(uint32_t eax, uint32_t edx, uint32_t esi);   // 0x8BE10: EAX = client, EDX = new gold, ESI = old
    void (*f_8bda0)(uint32_t eax, uint32_t edx, uint32_t esi);   // 0x8BDA0: EAX = client, EDX = new experience, ESI = old
};

uint32_t d2game_cc030_port(uint32_t player, uint32_t unit, uint32_t flag, const D2GameCC030Env* e);
