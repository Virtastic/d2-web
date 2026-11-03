#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69790: EAX = index (>= 16 fatal 0x65F), EDX = unit. Zeroes every second byte of
// data(+0x54)[0..0x1F], then frees the data+0x28 list (next +0x10, line 0x66D) and clears the head. No unit/data is fatal (0x668).
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69790(uint32_t idx, uint32_t unit, const Env* e) {
    if ((int32_t)idx >= 16) { e->fatal(0x65F); return; }
    for (uint32_t i = 0; i < 0x20; i += 2)
        if (unit && R(unit, 0x54)) B(R(unit, 0x54), i) = 0;
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x668); return; }
    for (uint32_t n = R(d, 0x28); n;) { uint32_t nx = R(n, 0x10); e->freeMem(n, 0x66D); n = nx; }
    W(d, 0x28) = 0;
}
