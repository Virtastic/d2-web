// 1.13c D2Client+0xb6ac0 -- client missile: ring of type-2 children around the unit, one per 16-unit offset pair of the two 8-entry tables (stride row+0x6c, at least 1); ECX = unit; returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b6ac0(uint32_t u, const cl::Env& e) {
    static const int32_t dx[8] = {0, 16, 16, 16, 0, -16, -16, -16};     // 1.13c D2Client+0xd3d2c
    static const int32_t dy[8] = {16, 16, 0, -16, -16, -16, 0, 16};     // 1.13c D2Client+0xd3d4c
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t q[23] = {};
    q[0] = 2; q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n < 1) n = 1;
    int32_t step = (int32_t)((uint32_t)n * 4u);
    for (int32_t i = 0; i < 0x20; i += step) {
        if (i < 0) break;
        q[7] = (uint32_t)dy[i >> 2];
        q[8] = (uint32_t)dx[i >> 2];
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) {
            e.H(0xc448, m, (uint32_t)dy[i >> 2]);
            e.H(0xc394, m, (uint32_t)dx[i >> 2]);
        }
    }
    return 1;
}
