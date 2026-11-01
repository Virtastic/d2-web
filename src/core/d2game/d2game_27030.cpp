// D2Game+0x27030: the leap-attack landing step. EAX = skill, stack: game, unit, x, y (ret 0x10). Returns 1 when the
// unit is sent on, 0 otherwise. Needs a skill and a path (unit +0x2C). The skill's saved target (param 3 type via
// 0xAD3C, param 4 id via 0xAB44) is looked up (0xE03A0); none: 0. Then the skill flags get 1 (0xA78A), the unit's
// animation frame 12 (0xACE8) and its +0x48 precise frame count byte 1; the path is recached (0xACEE), distance 5
// (0xAB14), step 1 (0xAD18), target cleared (0xA5FE). The point reflected through (x, y) away from the target,
// (3x - 2tx, 3y - 2ty), is looked up next to the unit's room (0x1280); a room there that is a town (0xA808) gives
// 0. Else the path gets that point (0xAB2C), mode 8 (0xAAFC), velocity 5 (0xAD24) and the unit as owner (0xAAF0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

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

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x27030
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_27030(uint32_t skill, uint32_t game, uint32_t unit, int32_t x, int32_t y, const D2Game27030Env* env)
{
    if (!skill) return 0;
    const uint32_t path = rd32(unit, 0x2c);
    if (!path) return 0;
    const uint32_t id = env->skillParam4(skill);
    const uint32_t type = env->skillParam3(skill);
    const uint32_t t = env->findUnit(type, game, id);
    if (!t) return 0;
    env->setSkillFlags(skill, 1);
    env->setAnimFrame(unit, 0xc);
    const uint32_t f = (rd32(unit, 0x48) & 0xffffff00u) + 0x100;
    std::memcpy((void*)(uintptr_t)(unit + 0x48), &f, 4);
    env->recache(path);
    env->setDistance(path, 5);
    env->setStepNum(path, 1);
    env->setTargetUnit(path, 0);
    const int32_t tx = (int32_t)((uint32_t)x * 3u - (uint32_t)env->unitX(t) * 2u);
    const int32_t ty = (int32_t)((uint32_t)y * 3u - (uint32_t)env->unitY(t) * 2u);
    const uint32_t room = env->roomAt(env->unitRoom(unit), tx, ty);
    if (room && env->isTown(room)) return 0;
    env->setTargetPos(path, tx, ty);
    env->setMode(path, 8);
    env->setVelocity(path, 5);
    env->setOwner(path, unit, 0);
    return 1;
}
