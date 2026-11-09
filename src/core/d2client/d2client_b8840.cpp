// 1.13c D2Client+0xb8840 -- client missile: periodic angled child. ECX = unit, no return.
// No unit / bad class / row +0x1E < 0: 0xB1720(ESI = unit). owner = 0xA60D0(EAX = unit) (not checked); n = max(row +0x58, 1).
// When 0xC4D2(unit) % n (signed) is > 0 the block is skipped. Otherwise block word 0 = 2, 1 = owner, 2 = unit, 11 = 0xC388(unit),
// 12 = 0xC3FA(unit), 4 = row +0x1E; v = |0xC322(unit) % 64|, word 7 = tbl 0xD3F10[v], word 8 = tbl 0xD3E10[v];
// 0xC448(unit, (row +0x5C + v) % 64 signed), then 0xB30F0(ECX = block). Always the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
namespace {
const int32_t kCos[64] = { 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
                           -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2, 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29 };   // 0xD3F10
const int32_t kSin[64] = { 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
                           0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2 };   // 0xD3E10
}
extern "C" void d2c_b8840(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t n = (int32_t)e.R(row + 0x58); if (n < 1) n = 1;
    if ((int32_t)e.H(0xc4d2, u) % n <= 0) {
        uint32_t q[23] = {};
        q[0] = 2; q[1] = own; q[2] = u;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        int32_t v = (int32_t)e.H(0xc322, u) % 64; if (v < 0) v = -v;
        q[7] = (uint32_t)kCos[v]; q[8] = (uint32_t)kSin[v];
        e.H(0xc448, u, (uint32_t)(((int32_t)e.R(row + 0x5c) + v) % 64));
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
