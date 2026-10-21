// src/core/d2game/d2game_batch_eb560.cpp -- 1.13c D2Game.dll 0xEB560 (waypoint section reader)
// transcribed from the cached Ghidra decompile. Callees are seams in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameEB560Env {
    uint8_t* (*ordinal10920)(void* unit);                                       // Ordinal_10920, returns record with 3 ptrs at +0x10
    void     (*ordinal10795)(uint32_t dst, const uint8_t* src, uint32_t len, uint32_t flag);  // Ordinal_10795
};

// 'this'=unit, param_1 = *cursor, param_2 = end, retaddr slot = out cursor (uses *param_1 section start).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EB560(void* unit, uint8_t** cursor, uint32_t end, uint32_t* out, const D2GameEB560Env* env) {  // 1.13c D2Game+0xEB560
    const uint8_t* p = *cursor;
    uint32_t magic, ver;
    if ((uintptr_t)p + 0x12aU > end) return 0xf;
    memcpy(&magic, p, 4); memcpy(&ver, p + 4, 4);
    if (magic != 0x216f6f57u || ver != 6) return 0xf;
    const uint8_t* rec = env->ordinal10920(unit);
    const uint8_t* src = p + 10;
    for (int i = 0; i < 3; ++i, src += 0x60) {
        uint32_t dst; memcpy(&dst, rec + 0x10 + 4 * i, 4);
        env->ordinal10795(dst, src, 0x60, 1);
    }
    *out = (uint32_t)(uintptr_t)p + 0x12aU;
    return 0;
}
