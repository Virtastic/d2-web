// src/core/d2game/d2game_batch_42d90.cpp -- 1.13c D2Game.dll 0x42D90, written from the retail
// decompile. Register arguments the cached decompile drops are named as seams (retail records are
// opaque host objects read at retail byte offsets).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2Game42D90Env {
    const uint8_t* dataTables;                                      // *sgptDataTables
    uint32_t (*fnA0630)(void* game, void* unit);                    // 0xA0630
    void     (*fn9FA30)(void* game, uint32_t* pX);                  // 0x9FA30 (writes the target word)
    uint32_t (*ord10080)(void* unit, int32_t zero, uint32_t v);     // D2Common #10080
    void*    (*fn6F930)(int32_t zero, uint32_t* request);           // 0x6F930 missile creator
};

// param_1 = game, param_2 = unit, param_3 = missile id; stk8/stk4 = Ghidra's uStack_8/uStack_4
// (caller registers copied into request words 0xF and 0xE).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_42D90(void* game, void* unit, int32_t id, uint32_t stk8, uint32_t stk4, const D2Game42D90Env* env) {   // 1.13c D2Game+0x42D90
    const uint8_t* dt = env->dataTables;
    if (id < 0 || id >= (int32_t)rd32(dt, 0xBA0)) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)(rd32(dt, 0xB98) + (uint32_t)id * 0x23C);
    if (!rec) return 0;
    int32_t idx = rd16s(rec, 0x48);
    if (idx < 0 || idx >= (int32_t)rd32(dt, 0xB6C)) return 0;
    if (unit) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
    if (!env->fnA0630(game, unit) || !unit) return 0;
    uint32_t target = 0;
    env->fn9FA30(game, &target);
    if (!target || env->ord10080(unit, 0, target) >= 0x65) return 0;
    uint32_t req[0x17] = {};
    req[0] = 1; req[1] = (uint32_t)(uintptr_t)unit; req[2] = (uint32_t)(uintptr_t)game;
    req[6] = target; req[0xE] = stk8; req[0xF] = stk4;
    return env->fn6F930(0, req) ? 1 : 0;
}
