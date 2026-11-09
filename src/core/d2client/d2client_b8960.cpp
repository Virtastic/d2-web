// 1.13c D2Client+0xb8960 -- client missile: delayed child at the unit. ECX = unit, no return.
// Row (class at +4 against sgptDataTables +0xB6C, base +0xB64, stride 0x1A4) with row +0x1E >= 0, 0xC4D2(u) >= row byte
// +0x135 and 0xC352(path +0x2C) != 0: x/y = 0x1630 / 0x1660(ECX = u), request word 0 = 1, 1 = 0xA60D0(EAX = u), 5/6 = x/y,
// 11/12 = 0xC388 / 0xC3FA(u), 4 = row +0x1E (signed), 13 = row +0x58; 0xB30F0(ECX = block). A child gets the unit's
// path values: 0xC3C4(child path, 0xC2D4(path)), 0xC41E(child path, 0xC1AE(u)), 0xC33A(child path, 0xC232(u)), then
// 0xC7E30(EAX = child, ECX = 0xC7EC0(EAX = u), EDX = 0; push 0). Every exit ends in 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b8960(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (row && e.R16s(row + 0x1e) >= 0 && (int32_t)e.H(0xc4d2, u) >= (int32_t)e.R8(row + 0x135)
        && e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t q[23] = {};
        q[0] = 1;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = x; q[6] = y;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[13] = e.R(row + 0x58);
        uint32_t c = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (c) {
            uint32_t v = e.H(0xc2d4, e.R(u + 0x2c));
            e.H(0xc3c4, e.R(c + 0x2c), v);
            v = e.H(0xc1ae, u);
            e.H(0xc41e, e.R(c + 0x2c), v);
            v = e.H(0xc232, u);
            e.H(0xc33a, e.R(c + 0x2c), v);
            uint32_t k = e.H(0xc7ec0, 0, 0, 0, 0, 0, u);
            e.H(0xc7e30, 0, 0, 0, 0, 0, c, k, 0);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
