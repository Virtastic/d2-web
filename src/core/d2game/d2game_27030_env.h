// Env of D2Game+0x27030 (d2game_27030.cpp): shared with the FN probe src/core/fn/d2game_27030.cpp.
#pragma once
#include <cstdint>

struct D2Game27030Env {
    uint32_t (*skillParam4)(uint32_t skill);                                  // thunk 0xAB44
    uint32_t (*skillParam3)(uint32_t skill);                                  // thunk 0xAD3C
    uint32_t (*findUnit)(uint32_t type, uint32_t game, uint32_t id);          // D2Game 0xE03A0 (EAX type, ECX game, EDX id)
    void (*setSkillFlags)(uint32_t skill, uint32_t f);                        // thunk 0xA78A
    void (*setAnimFrame)(uint32_t unit, uint32_t frame);                      // thunk 0xACE8
    void (*recache)(uint32_t path);                                           // thunk 0xACEE
    void (*setDistance)(uint32_t path, uint32_t d);                           // thunk 0xAB14
    void (*setStepNum)(uint32_t path, uint32_t n);                            // thunk 0xAD18
    void (*setTargetUnit)(uint32_t path, uint32_t t);                         // thunk 0xA5FE
    int32_t (*unitX)(uint32_t unit);                                          // D2Game 0x1100 (ECX unit)
    int32_t (*unitY)(uint32_t unit);                                          // D2Game 0x1130 (ECX unit)
    uint32_t (*unitRoom)(uint32_t unit);                                      // thunk 0xA478
    uint32_t (*roomAt)(uint32_t room, int32_t x, int32_t y);                  // D2Game 0x1280 (EAX room, EBX x, EDI y)
    uint32_t (*isTown)(uint32_t room);                                        // thunk 0xA808
    void (*setTargetPos)(uint32_t path, int32_t x, int32_t y);                // thunk 0xAB2C
    void (*setMode)(uint32_t path, uint32_t m);                               // thunk 0xAAFC
    void (*setVelocity)(uint32_t path, uint32_t v);                           // thunk 0xAD24
    void (*setOwner)(uint32_t path, uint32_t unit, uint32_t z);               // thunk 0xAAF0
};
