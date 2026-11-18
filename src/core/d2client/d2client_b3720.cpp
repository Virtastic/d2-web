// 1.13c D2Client+0xb3720 -- client missile trail spawn: ECX = missile unit; always returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b3720(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row) return 1;
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t seed = u + 0x20;
    uint32_t x0 = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, seed, x0);
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n < 1) n = 1;
    uint32_t px = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t py = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    uint32_t rx = e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)(n + n + 1), seed);
    uint32_t bx = rx + (px - (uint32_t)n);
    uint32_t ry = e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)(n + n + 1), seed);
    uint32_t by = ry + (py - (uint32_t)n);
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)e.R16s(row + 0x2c);
    q[7] = bx; q[8] = by;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    return 1;
}
