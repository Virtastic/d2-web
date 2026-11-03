// 1.13c D2Client+0x68EA0: EAX = unit. Frees every node of the unit-data list at
// data(+0x54)+0x28 (next at +0x10, line 0x830), then clears the head. No unit or no data is fatal (line 0x82A).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_68ea0(uint32_t unit, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x82A); return; }
    for (uint32_t n = R(d, 0x28); n;) { uint32_t next = R(n, 0x10); e->freeMem(n, 0x830); n = next; }
    W(d, 0x28) = 0;
}
