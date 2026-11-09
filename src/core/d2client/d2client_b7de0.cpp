// 1.13c D2Client+0xb7de0 -- client missile scatter: up to eight children with random velocity. EAX = n0, ESI = unit,
// stdcall(a0, class), ret 8. Class row missing: return. n = n0 * 7, count = min(n, 8) (none when <= 0). Each child: a fresh block
// word 1 = a0, 2 = unit, 11 = 0xC388(unit), 12 = 0xC3FA(unit), 4 = class; m = 0xB30F0(ECX = block). With m: ux/uy = inline rnd(n)
// - n/2, k = rnd(row +0x5C) (0 when <= 0), 0xC7B00(EAX = m), then m->+0x54->+0x30 gets +4/+8/+0xC = ux, uy, row +0x58 (<< 11),
// +0x10/+0x14/+0x18 = 0, 0, k << 11 and +0x1C/+0x20/+0x24 = 0, 0, row +0x60 << 11 (each group re-reads the two links).
#include "cl_env.h"
namespace {
uint32_t rnd(const cl::Env& e, uint32_t seed, int32_t n) {
    if (n <= 0) return 0;
    uint32_t lo = e.Lcg(seed);
    return (n & (n - 1)) ? lo % (uint32_t)n : lo & (uint32_t)(n - 1);
}
uint32_t mot(const cl::Env& e, uint32_t m) { uint32_t pa = e.R(m + 0x54); return pa ? e.R(pa + 0x30) : 0; }
}
extern "C" void d2c_b7de0(int32_t n0, uint32_t unit, uint32_t a0, int32_t cls, const cl::Env& e) {
    uint32_t row = e.Row(cls);
    if (!row) return;
    int32_t n = (int32_t)((uint32_t)n0 * 7u);
    int32_t cnt = n > 8 ? 8 : n;
    if (cnt <= 0) return;
    do {
        uint32_t q[23] = {};
        q[1] = a0; q[2] = unit;
        q[11] = e.H(0xc388, unit);
        q[12] = e.H(0xc3fa, unit);
        q[4] = (uint32_t)cls;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (!m) continue;
        int32_t half = n / 2;
        int32_t ux = (int32_t)rnd(e, unit + 0x20, n) - half;
        int32_t uy = (int32_t)rnd(e, unit + 0x20, n) - half;
        int32_t c5 = (int32_t)e.R(row + 0x5c);
        uint32_t k = rnd(e, unit + 0x20, c5);
        e.H(0xc7b00, 0, 0, 0, 0, 0, m);
        uint32_t mo = mot(e, m);
        if (mo) { e.W(mo + 4, (uint32_t)ux << 11); e.W(mo + 8, (uint32_t)uy << 11); e.W(mo + 0xc, e.R(row + 0x58) << 11); }
        mo = mot(e, m);
        if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, k << 11); }
        mo = mot(e, m);
        if (mo) { e.W(mo + 0x1c, 0); e.W(mo + 0x20, 0); e.W(mo + 0x24, e.R(row + 0x60) << 11); }
    } while (--cnt);
}
