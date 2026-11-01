// src/core/d2game/d2game_batch_1bbc0.cpp -- 1.13c D2Game.dll sub_1BBC0 written from the cached
// Ghidra decompile and the disassembly (stdcall pGame, pUnit, pCtx; ret 0xC). Retail records are read as
// 32-bit dwords at their retail byte offsets; the handler table at 0x6FD2F850 is a host seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "../prng.h"

extern "C" uint32_t D2Game_SEED_RollLimitedRandomNumber(d2::Seed* pSeed, int32_t nLimit);   // 1.13c D2Game+0x11D0

struct D2Game1BBC0Env {
    // table at 0x6FD2F850 (nullable): handler(ECX = pGame, EDX = pUnit, stack pCtx, pRec), ret 8
    uint32_t (*handlers[7])(const void* game, void* unit, const void* ctx, const void* rec);
};

static inline uint32_t rd32_1bbc0(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// 1.13c D2Game+0x1BBC0. unit+0x20 is the u64 seed; ctx -> g (dword at ctx+0), g+0x38 -> {count, records*};
// records are 0xC bytes, +0 = kind. Returns AL only (the other bits are residue).
static uint32_t sub_1BBC0(const void* game, void* unit, const void* ctx, const D2Game1BBC0Env* env)
{
    const uint8_t* g = (const uint8_t*)(uintptr_t)rd32_1bbc0(ctx, 0);
    const uint8_t* p1 = (const uint8_t*)(uintptr_t)rd32_1bbc0(g, 0x38);
    if (!p1) return 0;
    d2::Seed* seed = (d2::Seed*)((uint8_t*)unit + 0x20);
    uint32_t lo = (uint32_t)d2::roll(*seed);
    if (lo % 100 >= 0x42) return 0;
    int32_t count = (int32_t)rd32_1bbc0(p1, 0);
    if (count == 0) return 0;
    uint32_t idx = D2Game_SEED_RollLimitedRandomNumber(seed, count);
    const uint8_t* rec = (const uint8_t*)(uintptr_t)rd32_1bbc0(p1, 4) + idx * 0xc;
    int32_t k = (int32_t)rd32_1bbc0(rec, 0);
    if (k < 0 || k >= 7) return 0;
    if (!env->handlers[k]) return 0;
    return env->handlers[k](game, unit, ctx, rec) & 0xffu;
}

// FN probe: stdcall (pGame, pUnit, pCtx); ctx+4 carries the call-log blob (retail never reads it past +0).
// The five table handlers are recorded like the capture's hooks (count, then rva, eax, ecx, edx, ebx, esi, s0..s3 per call)
// and return 1, as the descriptor's hooks (V = 1) do.
static uint32_t* g_log1bbc0;
static uint32_t g_ret1bbc0;
template <uint32_t Rva> static uint32_t h1bbc0(const void* game, void* unit, const void* ctx, const void* rec)
{
    uint32_t* w = g_log1bbc0 + 1 + g_log1bbc0[0]++ * 10;
    w[0] = Rva; w[1] = 0; w[2] = (uint32_t)(uintptr_t)game; w[3] = (uint32_t)(uintptr_t)unit; w[4] = w[5] = 0;
    w[6] = (uint32_t)(uintptr_t)ctx; w[7] = (uint32_t)(uintptr_t)rec; w[8] = w[9] = 0;
    return g_ret1bbc0;
}
// 1.13c D2Game+0x1BBC0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1bbc0(uint32_t* r)
{
    D2Game1BBC0Env env{};
    env.handlers[1] = h1bbc0<0x16970>; env.handlers[2] = h1bbc0<0x19BB0>; env.handlers[3] = h1bbc0<0x19BA0>;
    env.handlers[4] = h1bbc0<0x19AD0>; env.handlers[5] = h1bbc0<0x19A00>;
    const uint8_t* game = (const uint8_t*)(uintptr_t)r[0];
    g_log1bbc0 = (uint32_t*)(uintptr_t)rd32_1bbc0((const void*)(uintptr_t)r[2], 4);
    g_log1bbc0[0] = 0;
    g_ret1bbc0 = 1;
    r[4] = sub_1BBC0(game, (void*)(uintptr_t)r[1], (const void*)(uintptr_t)r[2], &env);
}
