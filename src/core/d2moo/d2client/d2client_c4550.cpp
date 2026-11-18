// 1.13c D2Client+0xC4550 (ecx = unit, edx = skill id, [esp+4] = arg, ret 4): skill row lookup (none: 0); the row's signed word +0xEA is a missile index.
// In range of the missile count (+0xB6C) it runs 0xAFF10(index, 0x1630(unit), 0x1660(unit), id, arg; EDX = unit); 0x1660 is called first. Returns 1 on both paths.
#include "cl_env.h"
extern "C" uint32_t d2c_c4550(uint32_t u, int32_t id, uint32_t arg, const cl::Env& e) {
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!row) return 0;
    int32_t m = e.R16s(row + 0xea);
    if (m >= 0 && m < (int32_t)e.R(e.dt + 0xb6c)) {
        uint32_t w = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t v = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        e.H(0xaff10, (uint32_t)m, v, w, (uint32_t)id, arg, 0, 0, u);
    }
    return 1;
}
