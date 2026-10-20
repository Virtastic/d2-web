// src/core/d2game/d2game_sub_bf90.cpp -- 1.13c D2Game.dll +0xBF90, transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2game/decomp/0xBF90.c). Batch lane: implementation + marker only.
// Callees are seams in retail order: D2Common #10511 (thunk 0xAABA), #10243 (0xAB20), #10554 (0xAC94),
// #10748 (0xACDC), #11031 (0xAD0C), and 0x9EE50 called with its raw register/stack arguments
// (ecx=scratch, edx=local out buffer, then param, 1, caller's return slot, param_3).
#include <cstdint>
#include <emscripten.h>

struct D2GameBf90Env {
    uint32_t (*o10511)(void* unit);
    uint32_t (*fn9EE50)(void* scratch, void* out8, void* unit, uint32_t one, uint32_t retSlot, uint32_t arg3);
    void     (*o10243)(uint32_t h, uint32_t v);
    void     (*o10554)(uint32_t h, void* unit);
    void     (*o10748)(uint32_t h, uint32_t v);
    void     (*o11031)(uint32_t h, uint32_t v);
};

// 1.13c D2Game+0xBF90
// param_2 is the unit (edx); param_1 (ecx) is unused; unaff_retaddr is the caller's return slot, passed
// through to 0x9EE50 as stack data. Returns 0 when #10511 yields no handle, else 1.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_bf90(uint32_t /*param_1*/, void* unit, uint32_t param_3,
                                           uint32_t unaffRetaddr, const D2GameBf90Env* env)
{
    uint8_t out8[8];
    uint8_t scratch[16];
    const uint32_t h = env->o10511(unit);
    if (h == 0) return 0;
    const uint32_t r = env->fn9EE50(scratch, out8, unit, 1, unaffRetaddr, param_3);
    env->o10243(h, r);
    env->o10554(h, unit);
    env->o10748(h, r);
    env->o11031(h, h);
    return 1;
}
