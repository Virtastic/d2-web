// 1.13c D2Client+0x56A60 -- start a skill's cast animation (fastcall ECX = unit, EDX = skill; stack p1; ret 4). Skill out of the
// sgptDataTables skill-desc table (+0xB98, count +0xBA0, stride 0x23C) or a null row: 0. g = 0xC310 (unit), none: 0.
// 0xCC94 (unit, 1, skill, p1, &C, &B, &A) < 0: 0. Then 0xCB7A (g, B), 0xCB8C (g, A), r = 0xC1F0 (unit),
// r2 = 0x1400 (EAX = r, EBX = B, EDI = A), 0xCC8E (r2, B, A, 1, 0x100); 1.
#include "cl_env.h"
extern "C" uint32_t d2c_56a60(uint32_t unit, uint32_t skill, uint32_t p1, const cl::Env& e) {
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    if (!(e.R(e.dt + 0xb98) + skill * 0x23c)) return 0;
    uint32_t g = e.H(0xc310, unit);
    if (!g) return 0;
    uint32_t A = 0, B = 0, C = 0;
    e.Xs((uint32_t)(uintptr_t)&B, (uint32_t)(uintptr_t)&A);
    if ((int32_t)e.H(0xcc94, unit, 1, skill, p1, (uint32_t)(uintptr_t)&C) < 0) return 0;
    e.H(0xcb7a, g, B);
    e.H(0xcb8c, g, A);
    uint32_t r = e.H(0xc1f0, unit);
    uint32_t r2 = e.H(0x1400, 0, 0, 0, 0, 0, r, 0, 0, B);
    e.Edj(0x1400, A);
    e.H(0xcc8e, r2, B, A, 1, 0x100);
    return 1;
}
