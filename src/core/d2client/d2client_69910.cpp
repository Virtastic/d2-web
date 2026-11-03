#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69910: EAX = unit, ECX = key (-1 = 0x1AF0), EBX = type. Returns the first node of
// data(+0x54)+0x28 (next +0x10) with +0 == key and +4 == type, else 0. No unit/data is fatal (0x3AC).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69910(uint32_t unit, uint32_t key, uint32_t type, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x3AC); return 0; }
    if (key == 0xFFFFFFFFu) key = e->call(0x1AF0, 0, 0, 0, 0, 0, 0, 0);
    for (uint32_t n = R(d, 0x28); n; n = R(n, 0x10)) if (R(n, 0) == key && R(n, 4) == type) return n;
    return 0;
}
