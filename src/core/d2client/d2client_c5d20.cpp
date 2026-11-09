// 1.13c D2Client+0xC5D20: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. mi = #40550(EAX = index, stack unit) outside [0, count at +0xB6C) returns 0.
// #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0 (X, Y unread). A non-null unit gets word +0xC4 |= 0x40. o = #B99D0(mi, index, a3) == 0 returns 0.
// d = #C430(o); s = #C316(unit, row+0x118, index, a3) + #C7EC0(EAX = unit); h = d ? s / d (signed) : (s - sign(s)) >> 1; #C7E30(EAX = o, ECX = s, EDX = 0, stack 0).
// If o word +0x54 and its word +0x30 are non-null: [+0x10] = 0, [+0x14] = 0, [+0x18] = -h << 11. #C7FB0(o). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c5d20(uint32_t unit, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t mi = (int32_t)e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)cls);
    if (mi < 0 || mi >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    uint32_t xy[2] = {};
    e.Ed((uint32_t)(uintptr_t)&xy[0]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&xy[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    uint32_t o = e.H(0xb99d0, (uint32_t)mi, (uint32_t)cls, a3);
    if (!o) return 0;
    int32_t d = (int32_t)e.H(0xc430, o);
    uint32_t s = e.H(0xc316, unit, e.R(row + 0x118), (uint32_t)cls, a3);
    s += e.H(0xc7ec0, 0, 0, 0, 0, 0, unit);
    int32_t sv = (int32_t)s, h;
    if (d) h = (int32_t)((int64_t)sv / d);
    else h = (int32_t)(s - (uint32_t)(sv >> 31)) >> 1;
    e.H(0xc7e30, 0, 0, 0, 0, 0, o, s, 0);
    uint32_t q = e.R(o + 0x54);
    if (q) {
        uint32_t w = e.R(q + 0x30);
        if (w) {
            e.W(w + 0x10, 0);
            e.W(w + 0x14, 0);
            e.W(w + 0x18, (0u - (uint32_t)h) << 11);
        }
    }
    e.H(0xc7fb0, o);
    return 1;
}
