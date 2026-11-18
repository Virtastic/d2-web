// 1.13c D2Client+0xA2C90: stdcall(unit, skill index, a2, sw), ret 0x10; returns 0 or 1.
// Skill-use trigger: the row's +0xE8 effect (0xB9A80 or 0xA26B0 with the 0xA1340 position pair when sw), then the row's handler
// (+0xF4, or +0x16C when sw and > 1) from the table at D2Client+0xDEA48, 0xC5C2/0xA8630, the shared effects tail and the
// 0xC316 scale stored (clamped to 12) as the skill-effect end at 0x11985C = the 0x1197F8 start + scale when unit is the player.
// ours: env.gl = the handler table (0xDEA48; count at gl + 0x208), env.gl2 = the effect pair (0x1197F8; +0x64 = 0x11985C), env.self = 0x11BBFC.
#include "cl_a2.h"
extern "C" uint32_t d2cl_a2c90(uint32_t unit, int32_t idx, uint32_t a2, uint32_t sw, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t rec = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!rec) return 0;
    const uint32_t tbl = e.gl;
    const int32_t f4 = e.R16s(rec + 0xf4);
    if (f4 < 0 || f4 >= (int32_t)e.R(tbl + 0x208)) return 0;
    uint32_t flag = 0;
    if (e.R16s(rec + 0xe8) > 0) {
        uint32_t ebx = 0, edi = 0, X = 0, Y = 0, xo = 0, yo = 0;
        if (sw) {
            e.Ed((uint32_t)(uintptr_t)&xo);
            e.H(0xa1340, (uint32_t)(uintptr_t)&yo, 0, 0, 0, 0, 0, 0, 0, unit);
            ebx = xo; edi = yo;
            if (ebx && edi) {
                X = ebx; Y = edi;
                ebx -= e.H(0x1630, 0, 0, 0, 0, 0, 0, unit);
                edi -= e.H(0x1660, 0, 0, 0, 0, 0, 0, unit);
                X += ebx; Y += edi;
            }
        }
        uint32_t r;
        if (e.R8(e.bm + 4) & e.R8(rec + 4)) {
            const uint32_t w[8] = { (uint32_t)e.R16s(rec + 0xe8), ebx, edi, X, Y, (uint32_t)idx, a2, 0 };
            r = cl::b9a80(e, unit, w);
        } else {
            e.Xs(Y, 0);
            r = e.H(0xa26b0, (uint32_t)e.R16s(rec + 0xe8), (uint32_t)idx, a2, ebx, edi, unit, 0, 0, 0, X);
        }
        if (r) flag = 1;
        if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    }
    int32_t hi = e.R16s(rec + 0xf4);
    if (sw && e.R16s(rec + 0x16c) > 1) hi = e.R16s(rec + 0x16c);
    uint32_t hnd = 0;
    if (hi >= 0 && hi < (int32_t)e.R(tbl + 0x208)) hnd = e.R(tbl + 4u * (uint32_t)hi);
    if (hnd) {
        if (e.H(hnd, a2, 0, 0, 0, 0, 0, unit, (uint32_t)idx)) flag = 1;
    } else {
        if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
        flag = 1;
    }
    const uint32_t x = e.H(0xc5c2, unit, 0);
    if (x) e.H(0xa8630, 0, 0, 0, 0, 0, 0, 0, 0, 0, x);
    if (!flag) return 0;
    cl::a2Effects(e, unit, rec);
    const int32_t sc0 = (int32_t)e.H(0xc316, unit, e.R(rec + 0x190), (uint32_t)idx, a2);
    if (sc0 <= 0) return 1;
    if (e.self != unit) return 1;
    uint32_t sc = (uint32_t)sc0 > 0xc ? 0xc : (uint32_t)sc0;
    if (e.R8(rec + 0x10) == 0x12 && (e.R(unit + 0x38) & 0xffffff00u)) return 1;
    e.W(e.gl2 + 0x64, e.R(e.gl2) + sc);
    return 1;
}
