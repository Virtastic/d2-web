// 1.13c D2Client+0xb45d0 -- client missile: link spawn toward the hashed target unit; ECX = unit, EDX = skip flag; gl = the 128-slot unit hash (D2Client+0x10a008).
#include "cl_env.h"
extern "C" uint32_t d2c_b45d0(uint32_t u, uint32_t skip, const cl::Env& e) {
    if (skip) return 0;
    uint32_t key = e.H(0xc322, u);
    uint32_t n = e.R(e.gl + 4 * (key & 0x7f));
    while (n && e.R(n + 0xc) != key) n = e.R(n + 0xe4);
    if (!n) return 1;
    uint32_t q[23] = {};
    q[0] = 0xa021;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[4] = 0x160;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[19] = e.H(0xc406, n);
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[7] = e.PD(n, 0xc3be, 0xc);
    q[8] = e.PD(n, 0xc2ec, 0x10);
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) e.H(0xc448, m, e.R(n + 0xc));
    return 1;
}
