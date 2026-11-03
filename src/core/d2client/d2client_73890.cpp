// 1.13c D2Client+0x73890 -- queue a skill's missile request. ECX = unit, EDX = skill (plain ret).
#include "cl_env.h"
extern "C" void d2c_73890(uint32_t u /*ECX*/, int32_t skill /*EDX*/, const cl::Env& e) {
    e.H(0x4c500, 5, 0, 0, 0, 0, 0, u);
    if (skill < 0 || skill >= (int32_t)e.R(e.dt + 0xc4)) return;
    uint32_t rec = (uint32_t)skill * 0x3c + e.R(e.dt + 0xbc);
    if (!rec) return;
    int32_t m = e.R16s(rec + 0x3a);
    if (m < 0 || m >= (int32_t)e.R(e.dt + 0xb6c)) return;
    int32_t s = e.R16s(rec + 0x38);
    uint32_t q[0x17] = {};
    q[1] = q[2] = u; q[4] = (uint32_t)m; q[10] = s < 0 ? 0 : (uint32_t)s; q[11] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
