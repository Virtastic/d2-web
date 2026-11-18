// 1.13c D2Client+0xb92b0 -- client missile: random-pick sub-missile. ECX = unit, no return.
// own = 0xA60D0(EAX = u) first; 0xC352(path +0x2C) == 0 ends in 0xB17C0. Row guard (class against sgptDataTables
// +0xB6C, row +0x1E >= 0), then a second guard on the row +0x1E row, fail into 0xB1720(ESI = u). Request word 0 = 0x21,
// 1 = own, 5/6 = 0x1630/0x1660(ECX = u), 11/12 = 0xC388/0xC3FA(u), 4 = row +0x1E. When the +0x1E row's word +0x96 is
// non-zero, word 0 = 0x8021 and 19 = that word. With row +0x20 >= 0: row +0x22 >= 0 draws 0x1330(EAX = 3, ECX = u +0x20)
// (0: word 4 = row +0x20, 1: word 4 = row +0x22), else 0x1330(EAX = 2) picks row +0x20 on 0. Row +0x58 == 0 or
// 0x1330(EAX = row +0x58) != 0 sets word 0 bit 0x4000. Words 7/8 = 0x1630/0x1660 again; 0xB30F0(ECX = block); a child
// gets 0xC41E(child path, 0xC1AE(u)) and 0xC33A(child path, 0xC232(u)). Ends in 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b92b0(uint32_t u, const cl::Env& e) {
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t row = e.UnitRow(u);
        if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
        uint32_t q[23] = {};
        q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[0] = 0x21;
        q[1] = own;
        q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        int32_t sub = e.R16s(row + 0x1e);
        q[4] = (uint32_t)sub;
        uint32_t r2 = e.Row(sub);
        if (!r2) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
        int32_t w96 = e.R16s(r2 + 0x96);
        if (w96) { q[0] = 0x8021; q[19] = (uint32_t)w96; }
        if (e.R16s(row + 0x20) >= 0) {
            if (e.R16s(row + 0x22) >= 0) {
                uint32_t k = e.H(0x1330, 0, 0, 0, 0, 0, 3, u + 0x20);
                if (k == 0) q[4] = (uint32_t)e.R16s(row + 0x20);
                else if (k == 1) q[4] = (uint32_t)e.R16s(row + 0x22);
            } else if (e.H(0x1330, 0, 0, 0, 0, 0, 2, u + 0x20) == 0) {
                q[4] = (uint32_t)e.R16s(row + 0x20);
            }
        }
        uint32_t w58 = e.R(row + 0x58);
        if (w58 == 0 || e.H(0x1330, 0, 0, 0, 0, 0, w58, u + 0x20) != 0) q[0] |= 0x4000;
        q[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) {
            uint32_t a = e.H(0xc1ae, u);
            uint32_t b = e.H(0xc232, u);
            uint32_t p = e.R(m + 0x2c);
            e.H(0xc41e, p, a);
            e.H(0xc33a, p, b);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
