// Env of D2Game+0x26E40 (d2game_26e40.cpp): shared with the FN probe src/core/fn/d2game_26e40.cpp.
#pragma once
#include <cstdint>

struct D2Game26E40Env {
    int32_t (*skillParam4)(uint32_t skill);                          // thunk 0xAB44 (10631)
    void (*setSkillParam4)(uint32_t skill, int32_t v);               // thunk 0xAB20 (11031)
    uint32_t (*findWeapon)(uint32_t unit);                           // D2Game 0x372C0 (stdcall 1)
    int32_t (*attackSpeed)(uint32_t unit, uint32_t weapon);          // thunk 0xB336
    uint32_t (*dualWield)(uint32_t unit);                            // D2Game 0x26A30 (EAX unit)
};
