// storm+0x46d0 FUNCTION-mode probe: one argument, the block S (r[0]); blobs lg 0 (r[14]), S 1 (r[16]), S2 2 (r[18]).
// The element dtor 0x44C0 and the base teardown 0x4B80 are the hooked calls: the probe logs each as the capture does (ECX = element, stack word 0 = this).
#include <emscripten.h>
#include <cstdint>
#include "storm_hk.h"

struct S46D0Env { void (*elemDtor)(uint8_t* e); void (*baseDtor)(uint8_t* self); };   // src/core/storm/storm_n72.cpp
extern "C" void Storm_Sub46D0(const S46D0Env& e, uint8_t* self);

static HookEnv* gE;
static uint32_t gBase;
static uint32_t tg(uint32_t v) { return 0xB1000000u | (1u << 16) | (v - gBase); }   // a pointer into blob S (index 1)
static void elem46(uint8_t* el) { gE->h(0x44C0, 0, tg((uint32_t)(uintptr_t)el)); }
static void base46(uint8_t* self) { gE->h(0x4B80, 0, 0, 0, 0, 0, tg((uint32_t)(uintptr_t)self)); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_46d0(uint32_t* r) {
    static const uint32_t sc[] = { 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[14], sc);
    gE = &e;
    gBase = r[16];
    const S46D0Env env{ elem46, base46 };
    Storm_Sub46D0(env, (uint8_t*)(uintptr_t)r[0]);
    r[4] = 0;
}
