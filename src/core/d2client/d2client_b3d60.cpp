// 1.13c D2Client+0xb3d60 -- client missile: spawn type 0x25f-0x263 child one step in a random cardinal direction; EAX = unit, stack (arg), ret 4.
#include "cl_env.h"
extern "C" uint32_t d2c_b3d60(uint32_t u, uint32_t a, const cl::Env& e) {
    static const int32_t tx[4] = {1, 0, -1, 0}, ty[4] = {0, 1, 0, -1};
    uint32_t seed = u + 0x20;
    uint32_t speed = e.Lcg(seed) % 0x7d;
    uint32_t dir = e.Lcg(seed) & 3;
    uint32_t x = e.PD(u, 0xc3be, 0xc);
    uint32_t y = e.PD(u, 0xc2ec, 0x10);
    int32_t dx = tx[dir], dy = ty[dir];
    uint32_t sel = e.Lcg(seed) % 0x1e;
    uint32_t type = sel <= 3 ? 0x260 + sel : 0x25f;
    uint32_t q[23] = {};
    q[0] = 3; q[1] = a; q[4] = type; q[5] = x + dx; q[6] = y + dy;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[7] = (uint32_t)dy; q[8] = (uint32_t)(-dx);
    uint32_t n = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!n) return 0;
    uint32_t pa = e.R(n + 0x54);
    if (pa) { uint32_t m = e.R(pa + 0x30); if (m) { e.W(m + 4, 0); e.W(m + 8, 0); e.W(m + 0xc, speed << 11); } }
    e.H(0xc448, n, e.PD(u, 0xc3be, 0xc));
    return e.H(0xc394, n, e.PD(u, 0xc2ec, 0x10));
}
