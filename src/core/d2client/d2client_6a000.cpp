#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A000: EAX = unit, EDI = index. Gfx prologue (line 0x464); 0x1C10(unit id, idx) must be
// non-zero (else fatal 0x486). Unless data(+0x54)[idx*2] is already set: 0x69B40(unit, &frame, idx); non-zero sets data[idx*2] = 1 and returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_6a000(uint32_t unit, uint32_t idx, const Env* e) {
    gfxPro(e, unit, 0x464);
    if (!e->call(0x1C10, unitId(unit), 0, idx, 0, 0, 0, 0)) { e->fatal(0x486); return 0; }
    if (unit && R(unit, 0x54) && B(R(unit, 0x54), idx * 2)) return 0;
    if (!e->call(0x69B40, unit, 0, 0, 0, 0, 0xB3000000u, idx)) return 0;
    if (unit && R(unit, 0x54)) B(R(unit, 0x54), idx * 2) = 1;
    return 1;
}
