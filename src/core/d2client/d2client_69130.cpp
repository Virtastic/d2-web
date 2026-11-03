// 1.13c D2Client+0x69130: EAX = unit, ECX = id. 1 when a node of data(+0x54)+0x2C (next +0xA4)
// has +4 == id, else 0. No unit/data is fatal (line 0x1EA).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69130(uint32_t unit, uint32_t id, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x1EA); return 0; }
    for (uint32_t n = R(d, 0x2C); n; n = R(n, 0xA4))
        if (R(n, 4) == id) return 1;
    return 0;
}
