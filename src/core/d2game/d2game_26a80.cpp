// D2Game+0x26A80: sets the leap velocity on the unit's dynamic path (SKILLS_SetVelocityForLeap).
// EDI = unit, stack: skill (ret 4). Returns 0 without a path (+0x2C) or when the skill has no target position.
// The skill's flags gain 0x1101; the path is cleared (11164 / 11056), aimed at the skill's target (10286),
// typed 9, given the unit's run velocity (monsters: MonStats +0x34 short; players: CharStats +0x41 byte; << 8)
// and bound back to the unit (10334 via 0xAAF0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26A80Env {
    uint32_t dataTables;                                           // *sgptDataTables
    void (*skillSetFlags)(uint32_t skill, uint32_t flags);         // thunk 0xA78A
    void (*pathClearA)(uint32_t path, uint32_t v);                 // thunk 0xAB08
    void (*pathClearB)(uint32_t path, uint32_t v);                 // thunk 0xA682
    int32_t (*skillTargetX)(uint32_t skill);                       // thunk 0xAD12
    int32_t (*skillTargetY)(uint32_t skill);                       // thunk 0xACB2
    void (*pathSetTarget)(uint32_t path, int32_t x, int32_t y);    // thunk 0xAB2C
    void (*pathSetType)(uint32_t path, uint32_t type);             // thunk 0xAAFC
    void (*pathSetVelocity)(uint32_t path, int32_t v, uint32_t file, uint32_t line);  // thunk 0xACAC
    void (*pathBind)(uint32_t path, uint32_t unit, uint32_t v);    // thunk 0xAAF0
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x26A80
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_26a80(uint32_t unit, uint32_t skill, uint32_t srcFile, const D2Game26A80Env* env)
{
    const uint32_t path = rd32(unit, 0x2c);
    if (!path) return 0;
    env->skillSetFlags(skill, 0x1101);
    env->pathClearA(path, 0);
    env->pathClearB(path, 0);
    const int32_t x = env->skillTargetX(skill);
    const int32_t y = env->skillTargetY(skill);
    if (!x || !y) return 0;
    env->pathSetTarget(path, x, y);
    env->pathSetType(path, 9);
    const int32_t cls = (int32_t)rd32(unit, 4);
    int32_t vel;
    if (rd32(unit, 0) == 1) {   // monster: MonStats (dt+0xA78 rows of 0x1A8, count dt+0xA80)
        if (cls < 0 || cls >= (int32_t)rd32(env->dataTables, 0xa80)) return 0;
        const uint32_t rec = rd32(env->dataTables, 0xa78) + (uint32_t)cls * 0x1a8;
        int16_t s; std::memcpy(&s, (const void*)(uintptr_t)(rec + 0x34), 2);
        vel = (int32_t)s << 8;
    } else {                    // CharStats (dt+0xBC4 rows of 0xC4, count dt+0xBC8); retail reads it unchecked
        const uint32_t rec = (cls < 0 || cls >= (int32_t)rd32(env->dataTables, 0xbc8)) ? 0
                           : rd32(env->dataTables, 0xbc4) + (uint32_t)cls * 0xc4;
        vel = (int32_t)*(const uint8_t*)(uintptr_t)(rec + 0x41) << 8;
    }
    env->pathSetVelocity(path, vel, srcFile, 0x6c2);
    env->pathBind(path, unit, 0);
    return 1;
}
