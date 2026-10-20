// src/core/d2game/d2game_batch_ca20.cpp -- 1.13c D2Game.dll FUN_6fc2ca20, transcribed from the
// cached Ghidra decompile (build-wasm/ghidra-d2game/decomp/0xCA20.c). Base 0x6FC20000.
// Retail records are opaque, read by retail byte offsets. The callee 0x9EEB0 (register-argument
// callback fan-out, still UNMAPPED) and the two ordinals are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameCA20Env {
    void* dataTables;                        // sgptDataTables: +0xB98 table base, +0xBA0 count, 0x23C stride
    void (*ordinal10867)(uint32_t p);        // result discarded by retail
    void (*ordinal10750)(uint32_t p);        // result discarded by retail
    void (*func9EEB0)(uint32_t value);      // FUN_6fcbeeb0(value), value in ECX
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// Calls Ordinal_10867 then Ordinal_10750 on param_2[0xb] (unless kind is 2 or 4..5, or it is null),
// then looks up record param_3 in the 0x23C-stride table; a bad index, null record or param_4 < 1
// gives 0, else (param_4-1)*rec[+0x15C] + rec[+0x158]. Hands that to 0x9EEB0 and returns 1.
// 1.13c D2Game+0xCA20
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_ca20(uint32_t, const void* p2, int32_t p3, int32_t p4, const D2GameCA20Env* env)
{
    const int32_t kind = (int32_t)rd32(p2, 0);
    const uint32_t ptr = rd32(p2, 0x2C);
    const bool run = kind != 2 && (kind < 4 || kind > 5) && ptr != 0;
    if (run) env->ordinal10867(ptr);
    if (run) env->ordinal10750(ptr);
    uint32_t v = 0;
    if (p3 >= 0 && p3 < (int32_t)rd32(env->dataTables, 0xBA0) && p4 >= 1) {
        const uint8_t* rec = (const uint8_t*)(uintptr_t)rd32(env->dataTables, 0xB98) + (uint32_t)p3 * 0x23C;
        if (rec) v = (uint32_t)(p4 - 1) * rd32(rec, 0x15C) + rd32(rec, 0x158);
    }
    env->func9EEB0(v);
    return 1;
}
