// Vector replay entry point for s01-prng: run one the retail captures record and hand back the
// final state + FNV-1a of every draw, the same way the retail capture tool computed it.
#include <emscripten.h>
#include "prng.h"
#include "../../tools/vectorfmt/vectorfmt.h"

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_prng_check(uint32_t low, uint32_t high, uint32_t nMax, uint32_t count, uint32_t* out /* low, high, hash */) {
    d2::Seed s = { low, high };
    uint32_t h = D2VEC_FNV_INIT;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t r = nMax ? d2::rollLimited(s, (int32_t)nMax) : (uint32_t)d2::roll(s);
        h = d2vec_fnv1a_step(h, r);
    }
    out[0] = s.low; out[1] = s.high; out[2] = h;
}
