// 1.13c D2Client+0x69090: EAX = unit, ECX = key. Walks data(+0x54)+0x28 (next +0x10)
// for a node whose +0 equals the key; returns *(node[2] + 0x14), or 0.
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69090(uint32_t unit, uint32_t key) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) return 0;
    for (uint32_t n = R(d, 0x28); n; n = R(n, 0x10))
        if (R(n, 0) == key) return R(R(n, 8), 0x14);
    return 0;
}
