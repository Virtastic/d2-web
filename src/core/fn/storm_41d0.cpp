// storm+0x41d0 FUNCTION-mode probe: args in descriptor order (edi, own, base, flags).
// The allocator 0x1C580 is the logged stub returning blob R (blob index 1 -> r[16]); blob 0 = lg (storm_hk.h).
#include <emscripten.h>
#include <cstdint>
#include "storm_hk.h"
uint32_t Storm_Sub41D0(uint32_t own, uint32_t p2, uint32_t p3, uint32_t edi);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_41d0(uint32_t* r) {
    const uint32_t sc[] = { 0x1C580, r[16], 0, 0 };
    stormhk::Scope hk(r, 0, 2, sc);
    r[4] = Storm_Sub41D0(r[1], r[2], r[3], r[0]);
}
