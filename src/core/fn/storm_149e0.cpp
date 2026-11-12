// storm+0x149E0: FUNCTION-mode probe. r[0] = EBX (ids), r[1] = ESI (count), r[2] = any, r[3] = timeout; r[4] result.
// Blobs: lg 0 (hooklog), ids 1. GetTickCount is pinned at 5 and Sleep is a no-op (descriptor iat lines); the three
// pumps log one hook record each, as the capture's hooks do.
#include <emscripten.h>
#include <cstdint>
#include "storm_hk.h"

struct Storm149E0Seams {
    uint32_t (*tick)();
    void (*sleep)(uint32_t ms);
    void (*pump124b0)();
    void (*pump14250)();
    void (*pump13170)();
};
extern "C" uint32_t Storm_Sub149E0(const uint32_t* ids, uint32_t n, int32_t any, uint32_t timeout, const Storm149E0Seams& s);

static stormhk::Scope* g_hk;
static uint32_t Tick() { return 5; }
static void Nap(uint32_t) {}
static void P124b0() { g_hk->e.h(0x124B0, 0, 0, 0, 0, 0); }
static void P14250() { g_hk->e.h(0x14250, 0, 0, 0, 0, 0); }
static void P13170() { g_hk->e.h(0x13170, 0, 0, 0, 0, 0); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_149e0(uint32_t* r) {
    const uint32_t sc[] = { 0 };
    stormhk::Scope hk(r, 0, 2, sc);
    g_hk = &hk;
    const Storm149E0Seams s{ Tick, Nap, P124b0, P14250, P13170 };
    r[4] = Storm_Sub149E0((const uint32_t*)(uintptr_t)r[0], r[1], (int32_t)r[2], r[3], s);
    g_hk = nullptr;
}
