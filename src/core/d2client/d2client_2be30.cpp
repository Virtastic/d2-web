// 1.13c D2Client+0x2BE30 -- skill start with a targeted object: ECX = unit, EDX = skill index, [esp+4] = A, ret 4.
// Gates as 0x2BD20; marks the unit (+0xC4 |= 0x40). 0xA1340 (EBX = unit; stack word 0 = &T1, EDI = &T2) returns non-zero and fills T1, T2 (0: return 0).
// A 0xB30F0 request {kind 0x20, unit, unit, +0x10 class, +0x1C T2, +0x20 T1, +0x2C index, +0x30 A} yields s (0: return 0); its path (+0x2C, 0: return 0)
// gets 0xC38E(path, 0xE) and 0xC376(path, s, 0).
#include "cl_mb2.h"
extern "C" uint32_t d2c_2be30(uint32_t u, int32_t i, uint32_t A, const clm::Env& e) {
    if (!e.SkillRow(i)) return 0;
    int32_t C0 = (int32_t)e.H(0x40550, (uint32_t)i, 0, 0, 0, 0, u);
    if (C0 < 0 || C0 >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x40);
    uint32_t T[2] = { 0, 0 };   // T[0] = T1 (stack word 0), T[1] = T2 (the EDI out-pointer)
    e.ed(e.ctx, (uint32_t)(uintptr_t)&T[1]);
    if (!e.H(0xa1340, 0, 0, 0, u, 0, (uint32_t)(uintptr_t)&T[0])) return 0;
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = u; q[2] = u; q[4] = (uint32_t)C0; q[7] = T[1]; q[8] = T[0]; q[11] = (uint32_t)i; q[12] = A;
    uint32_t s = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!s) return 0;
    uint32_t path = e.R(s + 0x2c);
    if (!path) return 0;
    e.H(0xc38e, 0, 0, 0, 0, 0, path, 0xe);
    e.H(0xc376, 0, 0, 0, 0, 0, path, s, 0);
    return 1;
}
