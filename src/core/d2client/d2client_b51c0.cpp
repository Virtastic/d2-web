// 1.13c D2Client+0xB51C0 (ecx = unit): with a room, request type 0x8005 / class 0x178 at the unit's path position (words 11/12 = 0xC388/0xC3FA, word 19 = missile row 0x176 word +0x96).
// A child from 0xB30F0 gets 0xC448(child, 0xC322(unit)) and 0xC394(child, LCG step of the unit seed mod 0x23 + 10). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b51c0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.Row(0x175);   // retail: count > 0x175, row = base + 0x263F4 = index 0x175
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[0] = 0x8005; q[1] = room; q[4] = 0x178;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[19] = row ? (uint32_t)e.R16s(row + 0x96) : 0;
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    uint32_t c = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (c) {
        e.H(0xc448, c, e.H(0xc322, u));
        uint32_t l = e.Lcg(u + 0x20);
        e.H(0xc394, c, l % 0x23u + 10u);
    }
    return 1;
}
