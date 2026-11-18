// 1.13c D2Client+0xB0870 (ecx = unit): spawn the unit's table-row side units (row +0x2c once at the unit, row +0x2e
// scattered over the 8-neighbour ring); always returns 1.
#include <cstdint>
#include "cl_env.h"
namespace {
// module tables at D2Client+0xD3CB0: three {count, off[4]} groups, then the 8-step x and y ring offsets
const int32_t kGrp[3][5] = {{4, 0, 1, 4, 5}, {4, 0, 2, 4, 6}, {3, 0, 3, 5, 0}};
const int32_t kX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int32_t kY[8] = {0, 1, 1, 1, 0, -1, -1, -1};
}  // namespace
// 1.13c D2Client+0xB0870
extern "C" uint32_t d2c_b0870(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    uint32_t ty = e.R(u), p = e.R(u + 0x2c);
    bool direct = ty == 2 || ((int32_t)ty > 3 && (int32_t)ty <= 5);
    uint32_t v1, v2;                                                                // v1 -> y offset, v2 -> x offset
    if (direct) { v1 = e.R(p + 0xc); v2 = e.R(p + 0x10); }
    else { v1 = p ? e.H(0xc3be, p) : 0; v2 = p ? e.H(0xc2ec, p) : 0; }
    int32_t m0 = e.R16s(row + 0x2c), m1 = e.R16s(row + 0x2e);
    if (m0 >= 0) {
        uint32_t k = e.H(0xc3fa, u);
        uint32_t g = e.H(0xc388, u);
        e.H(0xb0450, (uint32_t)m0, 0, 0, g, k, 0, 0, 0, 0, u);
    }
    if (m1 < 0) return 1;
    auto step = [&]() {
        uint64_t pr = (uint64_t)e.R(u + 0x20) * 0x6ac690c5u + e.R(u + 0x24);
        e.W(u + 0x20, (uint32_t)pr); e.W(u + 0x24, (uint32_t)(pr >> 32));
        return (uint32_t)pr;
    };
    uint32_t r3 = step() % 3;
    uint32_t rnd = step() & 7;
    int32_t cnt = kGrp[r3][0];
    for (int32_t i = 0; i < cnt; i++) {
        uint32_t idx = ((uint32_t)kGrp[r3][1 + i] + rnd) & 7;
        uint32_t k = e.H(0xc3fa, u);
        uint32_t g = e.H(0xc388, u);
        uint32_t nu = e.H(0xb0450, (uint32_t)m1, 0, 0, g, k, 0, 0, 0, 0, u);
        if (nu) e.H(0xc478, e.R(nu + 0x2c), (uint32_t)kY[idx] + v1, (uint32_t)kX[idx] + v2, 0);
    }
    return 1;
}
