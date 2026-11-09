// 1.13c D2Client+0xb7b40 -- client missile scatter: a child at a random point inside a circle. EAX = class, stdcall(a0, unit, r), ret 0xC.
// Class row missing or 0xC1F0(unit) == 0: return. class < row +0x1E: class += 0x1380(ECX = seed, ESI = row1E - class + 1) and the row
// is looked up again. Block word 0 = 1, 1 = a0, 4 = class, 11 = 0xC388(unit), 12 = 0xC3FA(unit). Up to four tries: dx, dy = inline
// rnd(2r+1) - r (unit seed); inside r*r: x/y = dx/dy + the unit's path x/y, room = 0x1400(EAX = 0xC1F0(unit), EBX = x, EDI = y), a room
// with 0xC2E6(room, x, y, 0, 1) == 0 gives words 5/6 and m = 0xB30F0(ECX = block). With m: 0xC7B00(EAX = m), 0xC7E30(EAX = m,
// ECX = row +0x58 (doubled when r == 6), EDX = 0; 0), and m->+0x54->+0x30 gets +0x10 = +0x14 = 0, +0x18 = -row +0x5C (doubled) << 11.
#include "cl_env.h"
namespace {
uint32_t rnd(const cl::Env& e, uint32_t seed, int32_t n) {
    if (n <= 0) return 0;
    uint32_t lo = e.Lcg(seed);
    return (n & (n - 1)) ? lo % (uint32_t)n : lo & (uint32_t)(n - 1);
}
}
extern "C" void d2c_b7b40(int32_t cls, uint32_t a0, uint32_t unit, int32_t r, const cl::Env& e) {
    uint32_t row = e.Row(cls);
    if (!row) return;
    if (!e.H(0xc1f0, unit)) return;
    int32_t top = e.R16s(row + 0x1e);
    if (cls < top) {
        cls += (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, unit + 0x20, 0, 0, (uint32_t)(top - cls + 1));
        row = e.Row(cls);
        if (!row) return;
    }
    uint32_t q[23] = {};
    q[0] = 1; q[1] = a0; q[4] = (uint32_t)cls;
    q[11] = e.H(0xc388, unit);
    q[12] = e.H(0xc3fa, unit);
    int32_t rr = (int32_t)((uint32_t)r * (uint32_t)r), n = (int32_t)((uint32_t)r * 2u + 1u);
    for (int it = 1;; ++it) {
        int32_t dx = (int32_t)rnd(e, unit + 0x20, n) - r;
        int32_t dy = (int32_t)rnd(e, unit + 0x20, n) - r;
        if ((int32_t)((uint32_t)dy * (uint32_t)dy + (uint32_t)dx * (uint32_t)dx) <= rr) {
            uint32_t x = (uint32_t)dx + e.PD(unit, 0xc3be, 0xc);
            uint32_t y = (uint32_t)dy + e.PD(unit, 0xc2ec, 0x10);
            uint32_t rm = e.H(0xc1f0, unit);
            uint32_t room = e.H(0x1400, 0, 0, 0, 0, 0, rm, 0, 0, x);
            e.Edj(0x1400, y);
            if (room && !e.H(0xc2e6, room, x, y, 0, 1)) {
                q[5] = x; q[6] = y;
                uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
                if (m) {
                    e.H(0xc7b00, 0, 0, 0, 0, 0, m);
                    int32_t k;
                    if (r == 6) { e.H(0xc7e30, 0, 0, 0, 0, 0, m, e.R(row + 0x58) << 1, 0); k = -(int32_t)e.R(row + 0x5c) * 2; }
                    else { e.H(0xc7e30, 0, 0, 0, 0, 0, m, e.R(row + 0x58), 0); k = -(int32_t)e.R(row + 0x5c); }
                    uint32_t pa = e.R(m + 0x54);
                    if (pa) {
                        uint32_t mo = e.R(pa + 0x30);
                        if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, (uint32_t)k << 11); }
                    }
                    return;
                }
            }
        }
        if (it >= 4) return;
    }
}
