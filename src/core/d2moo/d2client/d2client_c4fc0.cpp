// 1.13c D2Client+0xC4FC0 (ecx = unit, edx = skill id, [esp+4] = arg, ret 4): skill row lookup (none: 0); 0x40550(EAX = id; unit) must land in [0, missile count +0xB6C) or it returns 0.
// A positive row word +0x10E runs 0x88A70(0, 0, 0, 0; EBX = word); then 0xA1540(0x222, id, arg; EAX = unit). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c4fc0(uint32_t u, int32_t id, uint32_t arg, const cl::Env& e) {
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!row) return 0;
    int32_t f = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)id);
    if (f < 0 || f >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    int32_t w = e.R16s(row + 0x10e);
    if (w > 0) e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)w);
    e.H(0xa1540, 0x222, (uint32_t)id, arg, 0, 0, u);
    return 1;
}
