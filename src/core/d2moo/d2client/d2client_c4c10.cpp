// 1.13c D2Client+0xC4C10: fastcall ECX = unit, EDX = skill id, stack s0 (ret 4).
// Creates the 0x40550 missile toward the 0xA1340 target and sets its offsets to the target minus the unit position.
#include "d2client_skillfx.h"
namespace d2c_skillfx {
u32 c4c10(const Env& e, P u, i32 cls, u32 s0) {
    if (cls < 0 || cls >= (i32)e.rd32(e.dt + 0xba0)) return 0;
    if (!(u32)(e.rd32(e.dt + 0xb98) + (u32)cls * 0x23c)) return 0;
    const i32 mi = e.f40550((u32)cls, u);
    if (mi < 0 || mi >= (i32)e.rd32(e.dt + 0xb6c)) return 0;
    i32 x = 0, y = 0;
    if (!e.a1340(u, &x, &y)) return 0;
    const P o = e.a15c0(u, (u32)mi, (u32)cls, s0);
    if (!o) return 0;
    const u32 dx = (u32)x - e.f1630(u);
    const u32 uy = e.f1660(u);
    e.c448(o, (i32)(0u - ((u32)y - uy)));
    e.c394(o, (i32)dx);
    return 1;
}
} // namespace d2c_skillfx
