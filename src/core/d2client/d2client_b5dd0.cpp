// 1.13c D2Client+0xb5dd0 -- client missile: a scatter of child missiles (Missiles row +0x2C class) around the unit;
// ECX = missile unit, always returns 1. Count = D2Common 0xC322; radius = max(row +0x6C, 3); each child gets a
// random frame delay 1..5 (block word 12, inline seed step) and a non-zero offset from 0x1330 (EAX = range, ECX = seed).
// The request block is built once and reused, so words 7/8/12 carry over between children.
#include "cl_env.h"
extern "C" uint32_t d2c_b5dd0(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row) return 1;
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[0] = 0x102; q[1] = room; q[2] = u;
    q[4] = (uint32_t)e.R16s(row + 0x2c);
    q[11] = e.H(0xc388, u);
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n < 3) n = 3;
    int32_t cnt = (int32_t)e.H(0xc322, u);
    if (cnt <= 0) return 1;
    uint32_t seed = u + 0x20;
    for (; cnt; --cnt) {
        q[12] = e.Lcg(seed) % 5 + 1;
        uint32_t x = e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)(n + n + 1), seed) - (uint32_t)n;
        q[7] = x;
        uint32_t y = e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)(n + n + 1), seed) - (uint32_t)n;
        q[8] = y;
        if (!x && !y) q[7] = e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)n, seed) + 1;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return 1;
}
