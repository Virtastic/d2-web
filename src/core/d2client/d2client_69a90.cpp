#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69A90: EAX = index (>= 16 returns), ECX = param. Mode = 0x1A80(): 0 = 0x69850,
// 1 = 0x69830, 2 = 0x69770, each tail-called with EDX = param; any other mode is fatal (0xD3E).
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69a90(uint32_t idx, uint32_t param, const Env* e) {
    if ((int32_t)idx >= 16) return;
    uint32_t m = e->call(0x1A80, 0, 0, 0, 0, 0, 0, 0);
    if (m == 0) e->call(0x69850, 0, 0, param, 0, 0, 0, 0);
    else if (m == 1) e->call(0x69830, 0, 0, param, 0, 0, 0, 0);
    else if (m == 2) e->call(0x69770, 0, 0, param, 0, 0, 0, 0);
    else e->fatal(0xD3E);
}
