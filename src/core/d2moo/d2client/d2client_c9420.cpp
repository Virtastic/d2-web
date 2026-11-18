// 1.13c D2Client+0xC9420 (ecx = unit, edx = skill id, [esp+4] = arg, ret 4): skill row lookup (none: 0); row word +0x80 must be in [0, count +0xC4) and word +0x110 in [0, count +0xBC0), else 0.
// A word +0x112 in [0, +0xBC0) with a nonzero 0xC1FC(unit, +0x80 word) replaces the +0x110 word as the value; then 0x6B300(unit, 2; EAX = value). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c9420(uint32_t u, int32_t id, uint32_t arg, const cl::Env& e) {
    (void)arg;
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!row) return 0;
    int32_t a = e.R16s(row + 0x80);
    if (a < 0 || a >= (int32_t)e.R(e.dt + 0xc4)) return 0;
    int32_t v = e.R16s(row + 0x110);
    int32_t n = (int32_t)e.R(e.dt + 0xbc0);
    if (v < 0 || v >= n) return 0;
    int32_t b = e.R16s(row + 0x112);
    if (b >= 0 && b < n && e.H(0xc1fc, u, (uint32_t)a)) v = e.R16s(row + 0x112);
    e.H(0x6b300, u, 2, 0, 0, 0, (uint32_t)v);
    return 1;
}
