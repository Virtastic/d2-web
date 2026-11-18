// 1.13c D2Client+0xb5380 -- client missile: spawn a type-0x179 child displaced by the guid-hashed target's offset; EAX = guid, EBX = unit, one stack word a (ret 4); gl = the 128-slot unit hash (D2Client+0x10aa08).
#include "cl_env.h"
extern "C" uint32_t d2c_b5380(uint32_t g, uint32_t u, uint32_t a, const cl::Env& e) {
    uint32_t n = e.R(e.gl + 4 * (g & 0x7f));
    while (n && e.R(n + 0xc) != g) n = e.R(n + 0xe4);
    if (!n) return 0;
    if (e.R(n) != 2) {                                   // assert path: message, fatal exit (the process ends; the port continues like the stub)
        uint32_t s = e.H(0xbf92, 0x5a);
        e.H(0xbf80, 0x6fb8359f, s);
        e.H(0x3664, 0xffffffffu);
    }
    uint32_t xu = e.PD(u, 0xc3be, 0xc), xn = e.PD(n, 0xc3be, 0xc);
    uint32_t dx = xu - xn;
    uint32_t yu = e.PD(u, 0xc2ec, 0x10), yn = e.PD(n, 0xc2ec, 0x10);
    uint32_t dy = yu - yn;
    uint32_t q[23] = {};
    q[1] = a; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = 0x179;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!m) return 0;
    e.H(0xc7b00, 0, 0, 0, 0, 0, 0, 0, 0, 0, m);
    uint32_t pa = e.R(m + 0x54);
    uint32_t mo = pa ? e.R(pa + 0x30) : 0;
    if (mo) { e.W(mo + 4, 0); e.W(mo + 8, 0); e.W(mo + 0xc, 0); }
    pa = e.R(m + 0x54); mo = pa ? e.R(pa + 0x30) : 0;
    if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, 0x1000); }
    pa = e.R(m + 0x54); mo = pa ? e.R(pa + 0x30) : 0;
    if (mo) { e.W(mo + 0x1c, 0); e.W(mo + 0x20, 0); e.W(mo + 0x24, 0); }
    e.H(0xc448, m, dx);
    return e.H(0xc394, m, dy);
}
