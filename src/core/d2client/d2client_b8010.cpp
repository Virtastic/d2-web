// 1.13c D2Client+0xb8010 -- client missile: four-corner spawn at frame == row +0x58. ECX = unit, no return.
// No unit / class < 0 / class >= row count / row +0x1E < 0: 0xB1720(ESI = unit). Else owner = 0xA60D0(EAX = unit);
// none, or 0xC4D2(unit) != row +0x58: tail 0xB17C0(ECX = unit). Otherwise x/y = 0x1630 / 0x1660(ECX = owner) and a
// request block (word 0 = 0x2001, 1 = owner, 4 = row +0x1E, 11 = 0xC388(unit), 12 = 0xC3FA(unit)); four times
// word 6 = y + tbl[0xD3DF0 + 4i], word 5 = x + tbl[0xD3E00 + 4i], then 0xB30F0(ECX = block); then the 0xB17C0 tail.
#include "cl_env.h"
extern "C" void d2c_b8010(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own || e.H(0xc4d2, u) != e.R(row + 0x58)) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    static const int32_t kX[4] = { 0, -1, 0, 1 };    // 0xD3DF0 (added to y)
    static const int32_t kY[4] = { -1, 0, 1, 0 };    // 0xD3E00 (added to x)
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, own);
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, own);
    uint32_t q[23] = {};
    q[0] = 0x2001; q[1] = own;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)e.R16s(row + 0x1e);
    for (int i = 0; i < 4; ++i) {
        q[6] = (uint32_t)kX[i] + y;
        q[5] = (uint32_t)kY[i] + x;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
