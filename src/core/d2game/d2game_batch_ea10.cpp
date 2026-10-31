// src/core/d2game/d2game_batch_ea10.cpp -- 1.13c D2Game.dll row 0x0EA10, from the cached decompile
// (build-wasm/ghidra-d2game/decomp/0xEA10.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
//
// Register-only argument: the unit arrives in EBX (the stack word is the game). The 0x8E490 lookup takes
// the unit in EAX (copied from EBX) and the fixed selector 3 in ESI. Unmapped callees are seams on
// D2GameEA10Env in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

// Host seams for the callees the disassembly names.
struct D2GameEA10Env {
    // 0xA42A, D2Common #10973 STATLIST_GetUnitStatValue(unit, stat, layer): a non-zero value is the divisor of 2500.
    uint32_t (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t layer);
    // 0x8E490 (EAX = unit, ESI = selector): non-zero when the unit's +0xDC list already holds a record of that type.
    int32_t  (*sub8E490)(uint32_t unit, uint32_t selector);
    // 0x8EA80(ECX = game, EDX = unit, kind, delay, a5, a6, a7): 5 stack words.
    void     (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t kind, uint32_t delay,
                         uint32_t a5, uint32_t a6, uint32_t a7);
};

// EA10 and EA70 differ only in the stat they read (0xFD / 0xFC); the caller lists differ.
static void ea_core(uint32_t game, uint32_t unit, uint32_t stat, const D2GameEA10Env* env) {
    const int32_t period = (int32_t)env->ordinal10973(unit, stat, 0);
    if (period == 0) return;
    if (env->sub8E490(unit, 3) != 0) return;
    const int32_t delay = (int32_t)(2500 / (int64_t)period) + (int32_t)rd32(game, 0xA8) + 1;
    env->sub8EA80(game, unit, 3, (uint32_t)delay, 0, 0, 0);
}

// 1.13c D2Game+0xea10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_ea10_port(uint32_t game, uint32_t unit, const D2GameEA10Env* env) {
    ea_core(game, unit, 0xFD, env);
}

// 1.13c D2Game+0xea70: the same body with stat 0xFC.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_ea70_port(uint32_t game, uint32_t unit, const D2GameEA10Env* env) {
    ea_core(game, unit, 0xFC, env);
}
