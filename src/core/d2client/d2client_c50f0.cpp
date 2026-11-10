// 1.13c D2Client+0xC50F0: fastcall(ECX = unit) + EDX and one stack word, both dead (EDX is stored over the stack word,
// which #C718's result then replaces before any read). Returns 0 unless unit and unit word +0 == 1. h = #C838(ECX = unit, EDX = 0x126); if 0:
// #C550(unit, 0x126, 1, 0, str, 0x35C) then #C838 again; still 0 returns 0. cls = #C42A(h, str, 0x364); r = #C718(unit, h, 1); n = #CB68(h); #CB50(h, n + 1);
// p2 = #CB6E(h); p3 = #CB9E(h). cls outside [0, count) or the row missing returns 0. flag = n >= #C316(unit, row+0x118, cls, r) (signed);
// c2 = #C316 again; flag |= #C4D8(unit, p2, p3) >= c2. Unit word +0x10 == 1: when flag, #C1F0(unit, #1630, #1660, 0x6FB74290, 0, 1), then #C370 on its result,
// and if that is nonzero #B1420(ECX = it, EAX = 0, stack 0); v = short row+0xF0. == 2: v = short row+0xEE + #1330(EAX = 2, ECX = unit+0x20). Else returns 1.
// flag and v > 0: #CB50(h, 0), #CB7A(h, #1630), #CB8C(h, #1660), #C4410(EAX = unit, v, cls, r, 1). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c50f0(uint32_t unit, const cl::Env& e) {
    const uint32_t kStr = 0xB2000000u | 0xD7048;
    if (!unit || e.R(unit) != 1) return 0;
    uint32_t h = e.H(0xc838, 0, 0, 0, 0, 0, 0, unit, 0x126);
    if (!h) {
        e.Xs(0x35c);
        e.H(0xc550, unit, 0x126, 1, h, kStr);
        h = e.H(0xc838, 0, 0, 0, 0, 0, 0, unit, 0x126);
        if (!h) return 0;
    }
    int32_t cls = (int32_t)e.H(0xc42a, h, kStr, 0x364);
    uint32_t r = e.H(0xc718, unit, h, 1);
    uint32_t n = e.H(0xcb68, h);
    e.H(0xcb50, h, n + 1);
    uint32_t p2 = e.H(0xcb6e, h);
    uint32_t p3 = e.H(0xcb9e, h);
    uint32_t flag = 0;
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t c1 = (int32_t)e.H(0xc316, unit, e.R(row + 0x118), (uint32_t)cls, r);
    if (!((int32_t)n < c1)) flag = 1;
    int32_t c2 = (int32_t)e.H(0xc316, unit, e.R(row + 0x118), (uint32_t)cls, r);
    if (!((int32_t)e.H(0xc4d8, unit, p2, p3) < c2)) flag = 1;
    int32_t v;
    uint32_t mode = e.R(unit + 0x10);
    if (mode == 1) {
        if (flag) {
            uint32_t b = e.H(0x1660, 0, 0, 0, 0, 0, 0, unit);
            uint32_t a = e.H(0x1630, 0, 0, 0, 0, 0, 0, unit);
            e.Xs(1);
            uint32_t rm = e.H(0xc1f0, unit, a, b, 0xB2000000u | 0xC4290, 0);
            uint32_t w = e.H(0xc370, rm);
            if (w) e.H(0xb1420, 0, 0, 0, 0, 0, 0, w);
        }
        v = e.R16s(row + 0xf0);
    } else if (mode == 2) {
        uint32_t x = e.H(0x1330, 0, 0, 0, 0, 0, 2, unit + 0x20);
        v = e.R16s(row + 0xee) + (int32_t)x;
    } else {
        return 1;
    }
    if (flag && v > 0) {
        e.H(0xcb50, h, 0);
        e.H(0xcb7a, h, e.H(0x1630, 0, 0, 0, 0, 0, 0, unit));
        e.H(0xcb8c, h, e.H(0x1660, 0, 0, 0, 0, 0, 0, unit));
        e.H(0xc4410, (uint32_t)v, (uint32_t)cls, r, 1, 0, unit);
    }
    return 1;
}
