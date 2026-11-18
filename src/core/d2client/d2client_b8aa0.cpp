// 1.13c D2Client+0xB8AA0 (ecx = unit): missile row with a non-negative +0x1E word and a room, and a nonzero 0xC352(path): builds a request block (type 0x21, or 0x4021 when a 4-range roll on the unit seed is nonzero; room; two 0x1630/0x1660 pairs; 0xC388/0xC3FA; class word row +0x1E) and queues it through 0xB30F0. Tails to 0xB17C0; the guard-failure paths run 0xB1720.
#include "cl_env.h"
extern "C" uint32_t d2c_b8aa0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!u || !row || e.R16s(row + 0x1e) < 0) return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    if (!e.H(0xa60d0, 0, 0, 0, 0, 0, u)) return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[0] = 0x21;
        if (e.H(0x1330, 0, 0, 0, 0, 0, 4, u + 0x20)) q[0] = 0x4021;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);   // [esp+0x1c] .. [esp+0x28] with the block at esp+8: words 5..8
        q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
