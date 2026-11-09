// 1.13c D2Game+0xE0570: are two units the same unit / the same owner. EAX = unit B, ECX = unit A, no stack arguments (plain ret).
// Addresses are 32-bit retail words.
#pragma once
#include <cstdint>
struct D2GameE0570Env {
    uint32_t (*findUnit)(uint32_t game, uint32_t type, uint32_t guid);   // D2Game+0xE03A0: ECX = game, EAX = type, EDX = guid
    uint32_t (*ordinal10666)(uint32_t a, uint32_t b);                    // D2Game+0xA6F4 thunk, stdcall(a, b)
    uint32_t (*sub9BEB0)(uint32_t unit);                                 // D2Game+0x9BEB0: EAX = unit, returns AX
};
uint32_t d2game_e0570_port(uint32_t eax, uint32_t ecx, const D2GameE0570Env* e);
