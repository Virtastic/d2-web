// 1.13c D2Client+0xb7220 -- client missile: unit-with-path release; ECX = unit; falls to the unit-destroy helper (0xB1720, ESI = unit) when the class row is invalid.
#include "cl_env.h"
extern "C" uint32_t d2c_b7220(uint32_t u, const cl::Env& e) {
    auto bad = [&]() { return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); };
    if (!u) return bad();
    int32_t cls = (int32_t)e.R(u + 4);
    if (cls < 0) return bad();
    uint32_t row = e.Row(cls);
    if (!row) return bad();
    if (e.R16s(row + 0x1e) < 0) return bad();
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[2] = u;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)(int32_t)e.R16s(row + 0x1e);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return e.H(0xb2eb0, 0, 0, 0, 0, 0, 0, u);
}
