#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69F20: EAX = unit, EDI = index. Gfx.cpp prologue (line 0x4BE), then
// table = #10688(unit id) (none = fatal 0x4C2); table[idx + 0x13F] must be 1 (else fatal 0x4DC). data(+0x54)[idx*2] already set = 0;
// else 0x69B40(unit, &frame, idx) (0 = fatal 0x4D4), data[idx*2] = 1, returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69f20(uint32_t unit, uint32_t idx, const Env* e) {
    gfxPro(e, unit, 0x4BE);
    uint32_t t = e->call(0xC226, 0, 0, 0, 0, 0, unitId(unit), 0);
    if (!t) { e->fatal(0x4C2); return 0; }
    if (B(t, idx + 0x13F) != 1) { e->fatal(0x4DC); return 0; }
    if (unit && R(unit, 0x54) && B(R(unit, 0x54), idx * 2)) return 0;
    if (!e->call(0x69B40, unit, 0, 0, 0, 0, 0xB3000000u, idx)) { e->fatal(0x4D4); return 0; }
    if (unit && R(unit, 0x54)) B(R(unit, 0x54), idx * 2) = 1;
    return 1;
}
