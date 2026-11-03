// 1.13c D2Client+0x73B60 -- unit level clamp (cap 0x4D) and refresh.
// ECX = unit (ESI), EDX = delta (EBP).
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE void d2client_73b60(const D2c73Env* e, uint32_t unit, uint32_t delta) {
    if (!unit) return;
    int32_t lvl = (int32_t)C(e, 0xc430, 0, 0, 0, 0, 0, 0, unit);
    if (lvl >= 0x4e) {
        lvl = 0x4d;
        C(e, 0xc4a2, 0, 0, 0, 0, 0, 0, unit, (uint32_t)lvl);
        C(e, 0xc2c8, 0, 0, 0, 0, 0, 0, unit, (uint32_t)lvl);
    }
    const uint32_t pd = rd32(e, unit + 0x2c);
    const uint32_t base = C(e, 0xc31c, 0, 0, 0, 0, 0, 0, pd);
    C(e, 0xc3b2, 0, unit + 0x20, base + delta);
    C(e, 0xc38e, 0, 0, 0, 0, 0, 0, pd, 0xa);
    C(e, 0xc3b8, 0, 0, 0, 0, 0, 0, pd, (uint32_t)lvl);
    C(e, 0xc376, 0, 0, 0, 0, 0, 0, pd, unit, 0);
}
