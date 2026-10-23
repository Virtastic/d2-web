// src/core/d2game/d2game_batch_ea10.cpp -- 1.13c D2Game.dll row 0x0EA10, from the cached decompile
// (build-wasm/ghidra-d2game/decomp/0xEA10.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
//
// Register-only arguments: the unit for the 0x8E490 lookup arrives in EAX and the selector in ESI; the
// second unit (0x8EA80's param_2) arrives in EBX. They are explicit parameters below. Unmapped callees
// are seams on D2GameEA10Env in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

// Host seams for the callees the decompile names.
struct D2GameEA10Env {
    // 0xA42A, D2Common #10973: no stack arguments; non-zero value is the divisor of 2500.
    uint32_t (*ordinal10973)(void);
    // 0x8E490: the smallest positive record value at +4 among the unit's +0xDC list nodes whose type
    // dword equals the ESI selector; 0 when none.
    int32_t  (*sub8E490)(void* eaxUnit, uint32_t selector);
    // 0x8EA80(unit, target, kind, delay, a5, a6, a7): 7-argument queue helper.
    void     (*sub8EA80)(uint32_t unit, uint32_t target, uint32_t kind, uint32_t delay,
                         uint32_t a5, uint32_t a6, uint32_t a7);
};

// 1.13c D2Game+0xea10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ea10(uint32_t param1, uint32_t ebxUnit, uint32_t eaxUnit,
                                            uint32_t esiSelector, const D2GameEA10Env* env) {
    const int32_t period = (int32_t)env->ordinal10973();
    if (period == 0) return;
    if (env->sub8E490((void*)(uintptr_t)eaxUnit, esiSelector) != 0) return;
    const int32_t delay = (int32_t)(2500 / (int64_t)period) + 1 + (int32_t)rd32(param1, 0xA8);
    env->sub8EA80(param1, ebxUnit, 3, (uint32_t)delay, 0, 0, 0);
}

// 1.13c D2Game+0xea70: byte-identical to 0x0EA10 (different callers only).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ea70(uint32_t param1, uint32_t ebxUnit, uint32_t eaxUnit,
                                            uint32_t esiSelector, const D2GameEA10Env* env) {
    const int32_t period = (int32_t)env->ordinal10973();
    if (period == 0) return;
    if (env->sub8E490((void*)(uintptr_t)eaxUnit, esiSelector) != 0) return;
    const int32_t delay = (int32_t)(2500 / (int64_t)period) + 1 + (int32_t)rd32(param1, 0xA8);
    env->sub8EA80(param1, ebxUnit, 3, (uint32_t)delay, 0, 0, 0);
}
