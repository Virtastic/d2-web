// 1.13c D2Client+0x74D50 -- skill do: hands (skill, level) on with EAX = unit; success is a nonzero answer.
#include "cl_skdo.h"
extern "C" uint32_t d2c_74d50(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    return e.H(0x74930, (uint32_t)sk, lvl, 0, 0, 0, u) != 0;
}
