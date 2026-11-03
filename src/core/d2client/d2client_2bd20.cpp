// 1.13c D2Client+0x2BD20 -- skill start with a client object at the 0xA5F70 position: ECX = unit, EDX = skill index, [esp+4] = A, ret 4.
// Gates as 0x2B530 (skills row, class from 0x40550 inside 0xB6C); marks the unit (+0xC4 |= 0x40); ebx = 0xA5F70() (0: return 0). A 0xB30F0 request
// {kind 1, unit, +0x10 class, +0x14/+0x18 = 0x1630/0x1660(ebx), +0x2C index, +0x30 A} yields an object s (0: return 0) that gets 0xC448(s, ebx[0]) and 0xC394(s, ebx[3]).
#include "cl_mb2.h"
extern "C" uint32_t d2c_2bd20(uint32_t u, int32_t i, uint32_t A, const clm::Env& e) {
    if (!e.SkillRow(i)) return 0;
    int32_t C0 = (int32_t)e.H(0x40550, (uint32_t)i, 0, 0, 0, 0, u);
    if (C0 < 0 || C0 >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x40);
    uint32_t ebx = e.H(0xa5f70);
    if (!ebx) return 0;
    uint32_t q[23] = {};
    q[0] = 1; q[1] = u; q[4] = (uint32_t)C0;
    q[5] = e.H(0x1630, 0, ebx); q[6] = e.H(0x1660, 0, ebx);
    q[11] = (uint32_t)i; q[12] = A;
    uint32_t s = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!s) return 0;
    e.H(0xc448, 0, 0, 0, 0, 0, s, e.R(ebx));
    e.H(0xc394, 0, 0, 0, 0, 0, s, e.R(ebx + 0xc));
    return 1;
}
