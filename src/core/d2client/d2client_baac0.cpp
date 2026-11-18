// 1.13c D2Client+0xbaac0 -- client missile: level-scaled timed child request. ECX = unit, returns 1 on every path.
// u == 0, a class outside [0, sgptDataTables +0xB6C) or row +0x24 (signed) < 0 return; owner = 0xA60D0(EAX = u),
// 0 returns. Request word 0 = 3, 5/6 = 0x1630/0x1660(ECX = u), 1 = owner, 4 = row +0x24, 11/12 = 0xC388/0xC3FA(u),
// 21 = D2Client+0xB0290 (the callback). n = row +0x6C + (0xC3FA(u) - 1) * row +0x70; when n <= 0:
// sk = 0x15B0(EAX = 0xC388(u)) after 0xC3FA(u) (0 returns), n = max(0xC316(owner, sk +0x144, id, lvl), 1).
// 0xA25C0(ESI = block; n).
#include "cl_env.h"
extern "C" uint32_t d2c_baac0(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x24) < 0) return 1;
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) return 1;
    uint32_t q[23] = {};
    q[0] = 3;
    q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    q[1] = own;
    q[4] = (uint32_t)e.R16s(row + 0x24);
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[21] = 0xB20B0290u;   // module address, recorded as 0xB2000000|rva
    uint32_t n = (e.H(0xc3fa, u) - 1u) * e.R(row + 0x70) + e.R(row + 0x6c);
    if ((int32_t)n <= 0) {
        uint32_t id = e.H(0xc388, u);
        uint32_t lv = e.H(0xc3fa, u);
        uint32_t sk = e.H(0x15b0, 0, 0, 0, 0, 0, id);
        if (!sk) return 1;
        n = e.H(0xc316, own, e.R(sk + 0x144), id, lv);
        if ((int32_t)n < 1) n = 1;
    }
    e.H(0xa25c0, n, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)q);
    if (e.blk) e.blk(e.ctx, 0xa25c0, (uint32_t)(uintptr_t)q, 23);
    return 1;
}
