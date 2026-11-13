// 1.13c D2Client+0xBA9A0 (ecx = unit): missile with a non-negative row +0x2C and a positive row +0x6C builds a type-3 child request (0x1630/0x1660 words, room, class word, 0xC388/0xC3FA coordinates; callback word set when row +0x70 is nonzero) and queues it through 0xA25C0 with the +0x6C count. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_ba9a0(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0 || (int32_t)e.R(row + 0x6c) <= 0) return 1;
    uint32_t q[23] = {};
    q[0] = 3;
    q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    q[11] = e.H(0xc388, u);
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[4] = (uint32_t)e.R16s(row + 0x2c);
    q[12] = e.H(0xc3fa, u);
    if (e.R(row + 0x70)) q[21] = 0xB2073B60u;   // module address, recorded as 0xB2000000|rva
    e.H(0xa25c0, e.R(row + 0x6c), 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)q);
    if (e.blk) e.blk(e.ctx, 0xa25c0, (uint32_t)(uintptr_t)q, 23);
    return 1;
}
