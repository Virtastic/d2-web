// 1.13c D2Client+0xC6290 (ecx = a, edx = skill id, [esp+4] = arg, ret 4): skill row (stride 0x23C, count +0xBA0) lookup; none returns 0.
// Otherwise 0xA0880(0xC316(a, row +0x114, id, arg)) and return 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c6290(uint32_t a, int32_t id, uint32_t arg, const cl::Env& e) {
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!row) return 0;
    e.H(0xa0880, e.H(0xc316, a, e.R(row + 0x114), (uint32_t)id, arg));
    return 1;
}
