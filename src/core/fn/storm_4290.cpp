// storm+0x4290 FUNCTION-mode probe: args in descriptor order (edi, own, base, flags).
// The allocator 0x1C580 is the logged stub returning blob E (blob index 1 -> r[16]); blob 0 = lg (storm_hk.h).
#include <emscripten.h>
#include <cstdint>
#include "storm_hk.h"
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
uint32_t* sub_04290(uint32_t* (*alloc)(uint32_t, int32_t, uint32_t), void (*link)(uint32_t*, int), int edi, uint32_t base, uint32_t flags);

static uint32_t* alloc4290(uint32_t n, int32_t line, uint32_t fl) { return (uint32_t*)Storm_Sub1C580(n, "AUEVENTREC", line, fl); }
static void link4290(uint32_t*, int) {}   // EDI is 0 in every vector, so the 0x4390 link never runs

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_4290(uint32_t* r) {
    const uint32_t sc[] = { 0x1C580, r[16], 0, 0 };
    stormhk::Scope hk(r, 0, 2, sc);
    r[4] = (uint32_t)(uintptr_t)sub_04290(alloc4290, link4290, (int)r[0], r[2], r[3]);
}
