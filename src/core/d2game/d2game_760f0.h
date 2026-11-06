// Callee seams of the 1.13c D2Game+0x760F0 port. 0xE03A0 and 0xF2380 are plain function pointers because the D2Game77dEnv one returns nothing.
#pragma once
#include <cstdint>
struct G760f0Env {
    uint32_t (*fE03A0)(uint32_t type, uint32_t id, uint32_t game);                                  // 0xE03A0: ECX = game, EAX = type, EDX = id; the unit or 0
    uint32_t (*t2A454)(uint32_t p);                                                                 // 0x2A454(p) stdcall
    uint32_t (*t2A3B8)(uint32_t p);                                                                 // 0x2A3B8(p) stdcall
    uint32_t (*t2A71E)(uint32_t unit, uint32_t stat, uint32_t layer);                               // 0x2A71E stdcall
    uint32_t (*t2A478)(uint32_t unit, uint32_t* out, uint32_t value);                               // 0x2A478(unit, out, value) stdcall
    uint32_t (*t2A694)(uint32_t v);                                                                 // 0x2A694(v) stdcall
    uint32_t (*t2A514)(uint32_t unit);                                                              // 0x2A514(unit) stdcall
    uint32_t (*fF2380)(uint32_t mode, uint32_t cmd, uint32_t game, uint32_t skill, uint32_t zero, uint32_t v1, uint32_t v2);   // 0xF2380: AL mode, ECX = cmd, EBX = game
    void     (*fDEEE0)(uint32_t game, uint32_t cmd, uint32_t zero, uint32_t one);                   // 0xDEEE0: ECX = game, EDX = cmd, 2 stack words
    void     (*f8E6F0)(uint32_t node, uint32_t game);                                               // 0x8E6F0: EAX = node, EDI = game
    uint32_t (*dataTables)(void);                                                                   // the sgptDataTables pointee (D2Game+0xF829C, two derefs)
};
extern "C" void d2game_760f0_port(const G760f0Env* e, uint32_t unit, uint32_t game);
