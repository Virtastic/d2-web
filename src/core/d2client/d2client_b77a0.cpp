// 1.13c D2Client+0xb77a0 -- client missile: type-9 child at the unit's position from the class row (+0x1e class, +0x58 extra), then the B17C0 tick; ECX = unit.
#include "cl_env.h"
extern "C" uint32_t d2c_b77a0(uint32_t u, const cl::Env& e) {
    auto bad = [&]() { return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); };
    if (!u) return bad();
    int32_t cls = (int32_t)e.R(u + 4);
    if (cls < 0) return bad();
    uint32_t row = e.Row(cls);
    if (!row) return bad();
    if (e.R16s(row + 0x1e) == 0) return bad();
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[0] = 9;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[13] = e.R(row + 0x58);
        q[4] = (uint32_t)(int32_t)e.R16s(row + 0x1e);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
