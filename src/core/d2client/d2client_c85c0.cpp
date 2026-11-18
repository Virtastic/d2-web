// 1.13c D2Client+0xC85C0 (ecx = unit, edx = skill id, [esp+4] = arg, ret 4): skill row lookup (none: 0); the row's signed word +0xEE is a missile index.
// In range of the missile count (+0xB6C) it runs 0xA15C0(index, id, arg; EAX = unit). Returns 1 on both of those paths.
#include "cl_env.h"
extern "C" uint32_t d2c_c85c0(uint32_t u, int32_t id, uint32_t arg, const cl::Env& e) {
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!row) return 0;
    int32_t m = e.R16s(row + 0xee);
    if (m >= 0 && m < (int32_t)e.R(e.dt + 0xb6c)) e.H(0xa15c0, (uint32_t)m, (uint32_t)id, arg, 0, 0, u);
    return 1;
}
