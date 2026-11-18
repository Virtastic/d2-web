// 1.13c D2Client+0xb70a0 -- client missile: periodic child of one of up to three Missiles classes. ECX = unit, no return.
// No unit / class / row: 0xB1720() and done. t = D2Common 0xC4D2(u). n = the leading run (at most 3) of row slots with
// class word row +0x1E+2k >= 0 and period row +0x5C+4k > 0; n == 0 calls 0xB1420(EAX = 0, ECX = u, push 0) and returns.
// t < row +0x58 skips to the tail. Otherwise the seed at u +0x20 is set by 0xC3B2 (ECX = seed, EDX = x), k = roll mod n,
// and when t is a multiple of slot k's period a request (type word 0, room, unit, class word k) is sent; a spawned
// missile gets 0xB1420(EAX = 0, ECX = it, push 1). Tail: 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b70a0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720); return; }
    int32_t t = (int32_t)e.H(0xc4d2, u);
    int32_t n = 0;
    while (n < 3 && e.R16s(row + 0x1e + 2 * n) >= 0 && (int32_t)e.R(row + 0x5c + 4 * n) > 0) ++n;
    if (n <= 0) { e.H(0xb1420, 0, 0, 0, 0, 0, 0, u); return; }
    if (t >= (int32_t)e.R(row + 0x58)) {
        uint32_t x = e.PD(u, 0xc3be, 0xc);
        e.H(0xc3b2, 0, 0, 0, 0, 0, 0, u + 0x20, x);
        uint32_t k = e.Lcg(u + 0x20) % (uint32_t)n;
        if (t % (int32_t)e.R(row + 0x5c + 4 * k) == 0) {
            uint32_t q[23] = {};
            q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
            q[2] = u;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[4] = (uint32_t)e.R16s(row + 0x1e + 2 * k);
            uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (m) e.H(0xb1420, 1, 0, 0, 0, 0, 0, m);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
