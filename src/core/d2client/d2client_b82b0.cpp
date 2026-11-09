// 1.13c D2Client+0xb82b0 -- client missile spawn with a random non-zero offset. EAX = unit, stdcall(a0, cls, r), ret 0xC, returns m.
// Block word 0 = 2, 1 = a0, 2 = unit, 4 = cls, 11 = 0xC388(unit), 12 = 0xC3FA(unit). rx, ry = inline rnd(2r+1) - r on the unit
// seed (+0x20); both zero: rx = 0x1330(EAX = r-1, ECX = seed) + 1. Words 7/8 = rx/ry, m = 0xB30F0(ECX = block);
// m: 0xC448(m, rx), 0xC394(m, ry).
#include "cl_env.h"
namespace {
uint32_t rnd(const cl::Env& e, uint32_t seed, int32_t n) {
    if (n <= 0) return 0;
    uint32_t lo = e.Lcg(seed);
    return (n & (n - 1)) ? lo % (uint32_t)n : lo & (uint32_t)(n - 1);
}
}
extern "C" uint32_t d2c_b82b0(uint32_t unit, uint32_t a0, uint32_t cls, uint32_t r, const cl::Env& e) {
    uint32_t q[23] = {};
    q[0] = 2; q[4] = cls; q[1] = a0; q[2] = unit;
    q[11] = e.H(0xc388, unit);
    q[12] = e.H(0xc3fa, unit);
    int32_t n = (int32_t)(r + r + 1u);
    uint32_t rx = rnd(e, unit + 0x20, n) - r;
    uint32_t ry = rnd(e, unit + 0x20, n) - r;
    if (!rx && !ry) rx = e.H(0x1330, 0, 0, 0, 0, 0, r - 1u, unit + 0x20) + 1u;
    q[7] = rx; q[8] = ry;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) { e.H(0xc448, m, rx); e.H(0xc394, m, ry); }
    return m;
}
