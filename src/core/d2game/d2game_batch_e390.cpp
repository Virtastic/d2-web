// src/core/d2game/d2game_batch_e390.cpp -- 1.13c D2Game.dll+0x0E390, transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2game/decomp/0xE390.c). Batch lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// EAX = pUnit (register argument). Looks up a value in a 5-entry-ish dword table (count DAT_6fd2f880,
// entries at +4, fallback at +0x10). Index = Ordinal_10459(Ordinal_10826(Ordinal_10331())), +2 when
// pUnit->+0x80 (pointer) is set and its byte +0x6D is nonzero. Null from 10331 or null pUnit is fatal:
// log via Ordinal_10265/10024 (codes 0xD41 / 0xA2A) then _exit(-1). Callees/globals are Env seams.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2GameE390Env {
    void*    (*getCtx)();                      // Ordinal_10331
    uint32_t (*ctxToKey)(void* ctx);           // Ordinal_10826
    uint8_t  (*keyToIndex)(uint32_t key);      // Ordinal_10459
    uint32_t (*errString)(uint32_t code);      // Ordinal_10265
    void     (*logError)(uint32_t str);        // Ordinal_10024(&DAT_6fd19ff9, str)
    uint32_t tableCount;                       // DAT_6fd2f880
    const uint32_t* table;                     // &DAT_6fd2f884
    uint32_t fallback;                         // DAT_6fd2f890
};

// 1.13c D2Game+0x0E390
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_e390(const uint8_t* pUnit, const D2GameE390Env* env)
{
    uint32_t code;
    void* ctx = env->getCtx();
    if (!ctx) {
        code = 0xd41;
    } else {
        uint8_t idx = env->keyToIndex(env->ctxToKey(ctx));
        if (pUnit) {
            const uint8_t* p = *(const uint8_t* const*)(pUnit + 0x80);
            if (p && p[0x6d]) idx = (uint8_t)(idx + 2);
            if (env->tableCount <= idx) return env->fallback;
            return env->table[idx];
        }
        code = 0xa2a;
    }
    env->logError(env->errString(code));
    exit(-1);
}
