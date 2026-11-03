// Env of D2Game+0x27820 (d2game_27820.cpp): the callees of the LeapAttack hit as seams, shared with the FN probe src/core/fn/d2game_27820.cpp.
#pragma once
#include <cstdint>

struct D2Game27820Env {
    const uint8_t* (*skillRecord)(int32_t id);                                   // skills.txt row id of sgptDataTables (+0xB98 rows, +0xBA0 count, 0x23C each), null when out of range
    int32_t (*overlayCount)();                                                    // sgptDataTables +0xBC0
    void* (*skillById)(void* unit, int32_t id);                                   // thunk 0xA3F4 (ECX unit, EDX id)
    void* (*findTarget)(void* game, void* skill, void* unit);                     // 0x272C0 (stack game, skill; EDI unit; ret 8)
    int32_t (*toHit)(void* unit, int32_t id, int32_t level);                      // thunk 0xACD6 (3 words)
    uint32_t (*resultFlags)(void* game, void* unit, void* target, int32_t toHit); // 0xDE5A0 (EAX target, EDI unit; stack game, toHit, 0)
    int32_t (*hitClass)(void* unit);                                              // thunk 0xA826 (ECX unit)
    int32_t (*formula)(void* unit, int32_t calc, int32_t id, int32_t level);      // thunk 0xAC9A (4 words)
    void (*rollElemental)(void* unit, uint8_t* dmg, int32_t id, int32_t level);   // 0x9F210 (EAX id, EDI unit; stack dmg, level)
    void (*allocCombat)(void* game, void* unit, void* target, uint8_t* dmg, uint8_t srcDam); // 0xDDDE0 (CL srcDam, EAX unit, EDI dmg; stack game, target)
    void (*drainDurability)(void* game, void* unit, void* target);                // 0xDED30 (stdcall 3)
    void (*setOverlay)(void* target, int32_t overlay);                            // thunk 0xA83E (target, overlay, 0)
    void* (*stateList)(void* target, int32_t state);                              // thunk 0xA5D4 (target, state)
    void (*detach)(void* target, void* stats);                                    // thunk 0xABB6
    void (*freeList)(void* stats);                                                // thunk 0xABAA
};
