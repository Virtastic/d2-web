// 1.13c D2Client+0xC4B00: fastcall ECX = unit, EDX = skill id, stack s0 (ret 4).
// Spawns the skill's +0xEA missile at the handle's x / y (with a 0x88A70 burst of +0x10E when positive), then its +0xEC missile.
#include "d2client_skillfx.h"
namespace d2c_skillfx {
static bool misOk(const Env& e, i32 i) { return i >= 0 && i < (i32)e.rd32(e.dt + 0xb6c); }
u32 c4b00(const Env& e, P a, i32 cls, u32 s0) {
    if (cls < 0 || cls >= (i32)e.rd32(e.dt + 0xba0)) return 0;
    const P cr = (P)(u32)(e.rd32(e.dt + 0xb98) + (u32)cls * 0x23c);
    if (!cr) return 0;
    const u32 h = e.c310(a);
    if (!h) return 0;
    const u32 x = e.cb68(h), y = e.cb6e(h);
    if (!x || !y) return 0;
    const i32 m1 = e.rd16s(cr + 0xea);
    if (misOk(e, m1)) {
        const P n = e.aff10(a, (u32)m1, x, y, (u32)cls, s0);
        if (!n) return 0;
        const i32 burst = e.rd16s(cr + 0x10e);
        if (burst > 0) e.f88a70((u32)burst, n, 0, 0, 0);
    }
    const i32 m2 = e.rd16s(cr + 0xec);
    if (misOk(e, m2)) e.aff10(a, (u32)m2, x, y, (u32)cls, s0);
    return 1;
}
} // namespace d2c_skillfx
