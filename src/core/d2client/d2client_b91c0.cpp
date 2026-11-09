// 1.13c D2Client+0xb91c0 -- client missile: child with a completion callback. ECX = unit, no return.
// Row guard (class against sgptDataTables +0xB6C, row +0x1E >= 0) fails into 0xB1720(ESI = u). When 0xC4D2(u) >= row
// byte +0x135 and 0xC352(path +0x2C) != 0: request word 0 = 1, 1 = 0xA60D0(EAX = u), 5/6 = 0x1630/0x1660(ECX = u),
// 11/12 = 0xC388/0xC3FA(u), 4 = row +0x1E (signed), 13 = max(row +0x58, 1), 21 = D2Client+0xB0F40 (the
// callback), 22 = u; 0xB30F0(ECX = block). Past the guard it ends in 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b91c0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    if ((int32_t)e.H(0xc4d2, u) >= (int32_t)e.R8(row + 0x135) && e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[0] = 1;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        int32_t w58 = (int32_t)e.R(row + 0x58);
        q[13] = (uint32_t)(w58 < 1 ? 1 : w58);
        q[21] = 0xB20B0F40u;   // module address, recorded as 0xB2000000|rva
        q[22] = u;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
