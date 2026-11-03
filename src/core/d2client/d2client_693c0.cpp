// 1.13c D2Client+0x693C0: EDX = unit. Not when unit+0xC4 has 0x10000; type 0 modes 0/0x11 and
// type 1 modes 0/0xC are skipped. Otherwise a type-0 unit is passed to callee 0x19830 (ECX = 7, EDX = unit, stack 0, 1).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_693c0(uint32_t unit, const Env* e) {
    if (R(unit, 0xC4) & 0x10000) return;
    uint32_t mode = R(unit, 0x10), type = R(unit, 0);
    if (type == 0) { if (mode == 0 || mode == 0x11) return; }
    else if (type == 1) { if (mode == 0 || mode == 0xC) return; }
    if (type == 0) e->call(0x19830, 0, 7, unit, 0, 0, 0, 1);
}
