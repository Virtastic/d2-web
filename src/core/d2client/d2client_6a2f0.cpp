#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A2F0: EAX = index (-1 = the #10175 kind), EDX = unit, ECX = EBX selector. Gfx prologue (line 0x502).
// Branch on unit type (6 when no unit): 1 = 0x6A180(idx; ESI = unit) then 0x69910(unit, idx, EBX); 0 = 0x69B40(unit, &frame, idx), flag data[idx*2] = 1, then
// 0x69910(unit, -1, EBX); 2 = 0x69F20(unit; EDI = idx) then 0x69910(unit, -1, EBX); other = 0. Returns the 0x69910 result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_6a2f0(uint32_t idx, uint32_t unit, uint32_t sel, const Env* e) {
    uint32_t kind = gfxPro(e, unit, 0x502);
    uint32_t type = unit ? R(unit, 0) : 6;
    if (idx == 0xFFFFFFFFu) idx = kind;
    if (type == 1) { e->call(0x6A180, idx, 0, 0, sel, unit, 0, 0); return e->call(0x69910, unit, idx, 0, sel, 0, 0, 0); }
    if (type == 0) {
        if (e->call(0x69B40, unit, 0, 0, 0, 0, 0xB3000000u, idx) && unit && R(unit, 0x54)) B(R(unit, 0x54), idx * 2) = 1;
        return e->call(0x69910, unit, 0xFFFFFFFFu, 0, sel, 0, 0, 0);
    }
    if (type == 2) { e->call(0x69F20, unit, 0, 0, 0, 0, 0, 0); return e->call(0x69910, unit, 0xFFFFFFFFu, 0, sel, 0, 0, 0); }
    return 0;
}
