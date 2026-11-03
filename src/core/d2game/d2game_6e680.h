// Seam for D2Game+0x6E680 (ECX = idA, EAX = idB, stack = game, id, param3 (a short); ret 0xC; always returns 1).
#pragma once
#include <cstdint>
struct D2Game6E680Env {
    uint32_t (*playerData)(uint32_t unit);                       // 0xA4C0
    uint32_t (*rowOf)(uint32_t id);                              // 0xA53E (-1 for none)
    uint32_t (*allowed)(uint32_t unit, uint32_t game);           // 0xD2C70: EAX = unit, stack = game
    uint32_t (*roleOk)(uint32_t ecx, uint32_t game);             // 0x69E50: CL = byte, stack = game; AL = result. ecx carries ebp+0x10 in its upper bits
    void (*setState)(uint32_t unit, uint32_t v);                 // 0xA4A2
    uint32_t (*textRow)(uint32_t cls);                           // 0xA520
    void (*finish)(uint32_t unit, uint32_t game, uint32_t other); // 0xC9CB0: EAX = unit, stack = game, other
    void (*notify)(uint32_t other, uint32_t a, uint32_t b);      // 0x8EA80: EDX = other, stack = 1, b, 0, 0, 0 (ret 0x14)
    void (*send)(const uint8_t* pkt, uint32_t defined, uint32_t client); // 0x8AB20: EAX = 7-byte packet (id 0x58), stack = client; `defined` bytes are written
    void (*fatal)(uint32_t line);
};
uint32_t d2game_port_6e680(uint32_t idA, uint32_t idB, uint32_t game, uint32_t id, uint32_t param3, const D2Game6E680Env* env);
