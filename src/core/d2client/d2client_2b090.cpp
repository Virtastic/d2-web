// 1.13c D2Client+0x2B090 -- pending-action worker: stdcall(unit), ret 4. unit + 0x14 = the player data; its +0x150 (armed flag),
// +0x154 (action), +0x158 (kind), +0x15C (value). Needs a type-0 unit with player data and the armed flag.
// gl+0 = DWORD 0x1197EC (clock), +4 = DWORD 0x119828 (cooldown, written), +8 = DWORD 0x11C2F4, +12 = DWORD 0x11C2F8 (failure latch).
// rec := 0xA4E20(EAX = value & 0x7F, ECX = value, EDX = module table 0x10A608 + (kind << 9), stack kind); 0x16A0(EAX = unit);
// action 0x13 on kind 5 arms the cooldown (clock + 500). A null rec or action 0 ends here. Then by kind:
//   0: 0x2AFB0(EAX = value, ECX = rec, stack unit, action, 0)        1: 0xAEA0(EAX = rec, ESI = unit, stack action, 1, value)
//   2: 0xAE30(EAX = rec, EBX = unit, ESI = action, stack 2, value)   other: rec + 0xC4 bit 1 clear latches 0x11C2F8 = 1, 0x11C2F4 = 0 and ends
// A nonzero result (kinds 0-2) or bit 1 set runs 0x2A7C0(EAX = unit, EBX = kind, stack action, 0).
#include "cl_env.h"
extern "C" void d2c_2b090(uint32_t unit, const cl::Env& e) {
    if (!unit || e.R(unit)) return;
    uint32_t pd = e.R(unit + 0x14);
    if (!pd || !e.R(pd + 0x150)) return;
    uint32_t action = e.R(pd + 0x154), kind = e.R(pd + 0x158), value = e.R(pd + 0x15c);
    uint32_t rec = e.H(0xa4e20, kind, 0, 0, 0, 0, value & 0x7f, value, 0xB2000000u | (0x10a608u + (kind << 9)));
    e.H(0x16a0, 0, 0, 0, 0, 0, unit);
    if (action == 0x13 && kind == 5) e.W(e.gl + 4, e.R(e.gl) + 500);
    if (!rec || !action) return;
    bool ok;
    switch (kind) {
    case 0: ok = e.H(0x2afb0, unit, action, 0, 0, 0, value, rec) != 0; break;
    case 1: ok = e.H(0x2aea0, action, 1, value, 0, 0, rec, 0, 0, 0, unit) != 0; break;
    case 2: ok = e.H(0x2ae30, 2, value, 0, 0, 0, rec, 0, 0, unit, action) != 0; break;
    default:
        if (!((e.R(rec + 0xc4) >> 1) & 1)) { e.W(e.gl + 12, 1); e.W(e.gl + 8, 0); return; }
        ok = true;
    }
    if (ok) e.H(0x2a7c0, action, 0, 0, 0, 0, unit, 0, 0, kind, 0);
}
