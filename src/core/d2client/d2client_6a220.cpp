#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A220: EAX = unit. Gfx prologue (line 0xD17), then the unit type ([unit], 6 for a null unit) is the kind: 0 = 0x69B40(unit, &frame, id)
// then data(+0x54)[id*2] = 1; 1 = 0x6A180(id; ESI = unit); 2 = 0x69F20(unit; EDI = id); else fatal 0xD26. id = the 0xC65E result.
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_6a220(uint32_t unit, const Env* e) {
    // retail keeps 0xC65E's result in a local ([esp+8] after the prologue) and passes THAT as the id of all three paths; unit[1] only rides the 0xC658 frame
    const uint32_t id = gfxPro(e, unit, 0xD17);
    const uint32_t kind = unit ? R(unit, 0) : 6u;   // retail: eax = unit ? [unit] : 6, switched after the prologue
    if (kind == 0) {
        if (e->call(0x69B40, unit, 0, 0, 0, 0, 0xB3000000u, id) && unit && R(unit, 0x54)) B(R(unit, 0x54), id * 2) = 1;
    } else if (kind == 1) e->call(0x6A180, id, 0, 0, 0, unit, 0, 0);
    else if (kind == 2) e->call(0x69F20, unit, 0, 0, 0, 0, 0, 0);
    else e->fatal(0xD26);
}
