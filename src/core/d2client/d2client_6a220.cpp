#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A220: EAX = unit. Gfx prologue (line 0xD17) gives the kind: 0 = 0x69B40(unit, &frame, id)
// then data(+0x54)[id*2] = 1; 1 = 0x6A180(id; ESI = unit); 2 = 0x69F20(unit; EDI = id); else fatal 0xD26. id = unit ? unit[1] : -1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_6a220(uint32_t unit, const Env* e) {
    uint32_t kind = gfxPro(e, unit, 0xD17), id = unitId(unit);
    if (kind == 0) {
        if (e->call(0x69B40, unit, 0, 0, 0, 0, 0xB3000000u, id) && unit && R(unit, 0x54)) B(R(unit, 0x54), id * 2) = 1;
    } else if (kind == 1) e->call(0x6A180, id, 0, 0, 0, unit, 0, 0);
    else if (kind == 2) e->call(0x69F20, unit, 0, 0, 0, 0, 0, 0);
    else e->fatal(0xD26);
}
