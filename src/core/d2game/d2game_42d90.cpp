// src/core/d2game/d2game_42d90.cpp -- 1.13c D2Game.dll 0x42D90, written from the retail disassembly.
// Register convention: ecx = game, edx = unit, stack (id, arg2), callee cleans 8. Callees are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2Game42D90Env {
    const uint8_t* dataTables;                                                  // *sgptDataTables
    uint32_t (*fnA0630)(uint32_t game, uint32_t unit);                          // 0xA0630 (stdcall game, unit)
    void     (*fn9FA30)(uint32_t game, uint32_t unit, uint32_t* x, uint32_t* y);   // 0x9FA30 (eax unit, ecx game, ebx &x, stack &y)
    uint32_t (*dist)(uint32_t unit, uint32_t x, uint32_t y);                    // 0xAB80 (D2Common #10080)
    uint32_t (*create)(uint32_t game, const uint32_t* request);                 // 0x6F930 (ecx game, edx request)
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_42D90(uint32_t game, uint32_t unit, int32_t id, uint32_t arg2, const D2Game42D90Env* env) {   // 1.13c D2Game+0x42D90
    const uint8_t* dt = env->dataTables;
    if (id < 0 || id >= (int32_t)rd32(dt, 0xBA0)) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)(rd32(dt, 0xB98) + (uint32_t)id * 0x23C);
    if (!rec) return 0;
    int32_t mi = rd16s(rec, 0x48);
    if (mi < 0 || mi >= (int32_t)rd32(dt, 0xB6C)) return 0;
    if (unit) { uint8_t* u = (uint8_t*)(uintptr_t)unit; uint32_t f = rd32(u, 0xC4) | 0x40; std::memcpy(u + 0xC4, &f, 4); }
    if (!env->fnA0630(game, unit)) return 0;
    if (!unit) return 0;
    uint32_t x = 0, y = 0;
    env->fn9FA30(game, unit, &x, &y);
    if (!x && !y) return 0;
    if (env->dist(unit, x, y) > 0x64) return 0;
    uint32_t req[0x17] = {};
    req[0] = 1; req[1] = unit; req[4] = (uint32_t)mi; req[5] = x; req[6] = y; req[0xB] = (uint32_t)id; req[0xC] = arg2;
    return env->create(game, req) ? 1 : 0;
}
