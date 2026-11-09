// Env of D2Game+0x27A10 (d2game_27a10.cpp): the callees of the Frenzy damage roll as seams, shared with the FN probe src/core/fn/d2game_27a10.cpp.
#pragma once
#include <cstdint>

struct D2Game27a10Env {
    const uint8_t* (*skillRecord)(int32_t id);                                    // skills.txt row of sgptDataTables (+0xB98 rows, +0xBA0 count, 0x23C each), null when out of range
    void (*setTarget)(void* unit, void* target);                                  // thunk 0xACD0 (2 words)
    int32_t (*toHit)(void* unit, int32_t id, int32_t level);                      // thunk 0xACD6 (3 words)
    uint32_t (*resultFlags)(void* game, void* unit, void* target, int32_t toHit); // 0xDE5A0 (EAX target, EDI unit; stack game, toHit, 0)
    int32_t (*formula)(void* unit, int32_t calc, int32_t id, int32_t level);      // thunk 0xAC9A (4 words)
    void (*rollElemental)(void* unit, uint8_t* dmg, int32_t id, int32_t level);   // 0x9F210 (EAX id, EDI unit; stack dmg, level)
    void (*allocCombat)(void* game, void* unit, void* target, uint8_t* dmg, uint8_t srcDam); // 0xDDDE0 (CL srcDam, EAX unit, EDI dmg; stack game, target)
    void (*drainDurability)(void* game, void* unit, void* target);                // 0xDED30 (stdcall 3)
    void* (*usedSkill)(void* unit);                                               // thunk 0xAABA (1 word)
    void (*setParam1)(void* skill, int32_t v);                                    // thunk 0xAC94 (2 words)
};
