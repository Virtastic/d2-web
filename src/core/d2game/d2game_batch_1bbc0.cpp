// src/core/d2game/d2game_batch_1bbc0.cpp -- 1.13c D2Game.dll sub_1BBC0 transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x1BBC0.c). Retail data (callback table at
// 0x6FD2F850) and the 0x11D0 callee arrive as host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game1BBC0Env {
    uint32_t (*sub11D0)(uint64_t* seed);                          // 0x11D0, returns an index
    uint32_t (*handlers[7])(const void* ctx, const void* rec);   // table at 0x6FD2F850 (nullable)
};

// 1.13c D2Game+0x1BBC0. seedBase = param_2 (+0x20 u64 seed state, +0x24 high dword); ctx = param_3;
// piVar1 = *(*ctx + 0x38) -> {count, records*, ...}; records are 0xC bytes, +0 = kind.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_1bbc0(uint32_t eax, uint8_t* seedBase, const uint32_t* const* ctx, const D2Game1BBC0Env* env) {
    auto rd32 = [](const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    const uint8_t* g = (const uint8_t*)*(const uint8_t* const*)ctx;
    const uint8_t* p1 = (const uint8_t*)(uintptr_t)rd32(g + 0x38);
    if (!p1) return eax & 0xffffff00u;
    uint64_t s; std::memcpy(&s, seedBase + 0x20, 8);
    s = (uint64_t)(uint32_t)s * 0x6ac690c5ull + rd32(seedBase + 0x24);
    std::memcpy(seedBase + 0x20, &s, 8);
    uint32_t lo = (uint32_t)s;
    eax = lo / 100;
    if (lo % 100 >= 0x42) return eax & 0xffffff00u;
    if (rd32(p1) == 0) return eax & 0xffffff00u;
    uint32_t idx = env->sub11D0((uint64_t*)(seedBase + 0x20));
    const uint8_t* rec = (const uint8_t*)(uintptr_t)rd32(p1 + 4) + idx * 0xc;
    int32_t k = (int32_t)rd32(rec);
    if (k < 0 || k >= 7) return (uint32_t)(uintptr_t)rec & 0xffffff00u;
    if (!env->handlers[k]) return (uint32_t)(uintptr_t)rec & 0xffffff00u;
    return env->handlers[k](ctx, rec);
}
