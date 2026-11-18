// 1.13c D2Client+0xb5930 -- client missile: up to three child spawns from the class row's word fields +0x2c/+0x2e/+0x30; ECX = unit; returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b5930(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t cls = (int32_t)e.R(u + 4);
    if (cls < 0) return 1;
    uint32_t row = e.Row(cls);
    if (!row) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    for (uint32_t off = 0x2c; off <= 0x30; off += 2) {
        int32_t v = e.R16s(row + off);
        if (v < 0) continue;
        q[4] = (uint32_t)v;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return 1;
}
