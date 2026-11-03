// Env of D2Game+0x27B90 (d2game_27b90.cpp): shared with the FN probe src/core/fn/d2game_27b90.cpp.
#pragma once
#include <cstdint>

struct D2Game27B90Env {
    void (*checkUnit)(uint32_t unit, uint32_t game);                               // D2Game 0xE07A0 (EAX unit, ECX game)
    uint32_t (*pathTargetUnit)(uint32_t path);                                     // thunk 0xA598
    int32_t (*unitLevel)(uint32_t unit);                                           // thunk 0xA77E
    void (*areaSearch)(uint32_t game, uint32_t* blk, uint32_t unit, uint32_t a1, uint32_t a2, int32_t range,
                       uint32_t flags, uint32_t callback);                         // D2Game 0xA0C70 (EAX game, ECX blk, 8 stack)
    void (*faceTarget)(uint32_t unit, uint32_t target);                            // thunk 0xACD0
    void (*strike)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b);          // D2Game 0x273C0 (ECX game, EDX unit, 2 stack)
    void (*drainDurability)(uint32_t game, uint32_t unit, uint32_t target);        // D2Game 0xDED30 (stdcall 3)
    uint32_t callback;                                                             // D2Game 0x9ED70
};
