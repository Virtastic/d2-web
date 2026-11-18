// 1.13c D2Client+0xC4CE0: fastcall ECX = unit, EDX = skill id, stack s0 (ret 4).
// Lays the skill's +0xEC missile along the line to the 0xA1340 target every missiles[+0x38] units of the 10080 distance (each one
// delayed by missiles[+0x3C] more frames), then the +0xEA head missile through 0xA1540 delayed past the last of them.
#include "d2client_skillfx.h"
namespace d2c_skillfx {
static i32 div100(u32 v) { return (i32)v / 100; }
u32 c4ce0(const Env& e, P u, i32 cls, u32 s0) {
    if (cls < 0 || cls >= (i32)e.rd32(e.dt + 0xba0)) return 0;
    const P cr = (P)(u32)(e.rd32(e.dt + 0xb98) + (u32)cls * 0x23c);
    if (!cr) return 0;
    const i32 ri = e.rd16s(cr + 0xec);
    if (ri < 0 || ri >= (i32)e.rd32(e.dt + 0xb6c)) return 0;
    const P row = (P)(u32)(e.rd32(e.dt + 0xb64) + (u32)ri * 0x1a4);
    if (!row) return 0;
    i32 X = 0, Y = 0;
    if (!e.a1340(u, &X, &Y)) return 0;
    i32 total = e.c4d8(u, X, Y);
    if (total < 1) total = 1;
    const u32 ux = e.f1630(u), uy = e.f1660(u);
    const i32 dxs = (i32)(((u32)X - ux) * 100u) / total;
    const i32 dys = (i32)(((u32)Y - uy) * 100u) / total;
    i32 step = (i32)e.rd32(row + 0x38);
    if (step < 1) step = 1;
    const i32 inc0 = (i32)e.rd32(row + 0x3c), inc = inc0 >= 1 ? inc0 : 1;
    u32 n = 0;
    if (step < total) {
        const u32 ax0 = (u32)step * (u32)dxs, ay0 = (u32)step * (u32)dys;
        u32 ax = ax0, ay = ay0, acc = 0; i32 k = step;
        do {
            const P m = e.aff10(u, (u32)ri, (u32)div100(ax) + ux, (u32)div100(ay) + uy, (u32)cls, s0);
            if (m) {
                const u32 c = e.c610(u);
                e.c2b6(e.rd32(m + 0x2c), c);
                const u32 b = (u32)e.c430(m);
                e.c4a2(m, (i32)(acc + b));
                e.c2c8(m, (i32)(acc + b));
                e.c466(m, (i32)(b - acc));
                e.wr32(m + 0xc8, e.rd32(m + 0xc8) | 0x40000);
            }
            k = (i32)((u32)k + (u32)step); ay += ay0; acc += (u32)inc; ax += ax0; ++n;
        } while (k < total);
    }
    const i32 hi = e.rd16s(cr + 0xea);
    if (hi >= 0 && hi < (i32)e.rd32(e.dt + 0xb6c)) {
        const P o = e.a1540(u, (u32)hi, (u32)cls, s0);
        if (!o) return 0;
        const i32 v = e.c316(u, e.rd32(cr + 0x138), (u32)cls, s0);
        if (v > 0) e.c2fe(o, v);
        const u32 b = (u32)e.c430(o), d = n * (u32)inc;
        e.c4a2(o, (i32)(d + b));
        e.c2c8(o, (i32)(d + b));
        e.c466(o, (i32)(b - d));
        e.wr32(o + 0xc8, e.rd32(o + 0xc8) | 0x40000);
    }
    return 1;
}
} // namespace d2c_skillfx
