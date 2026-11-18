// 1.13c D2Client+0xB2980 (ecx = unit, void): a stat below 100 (ordinal 10737) is raised to 500 (ordinal 10970); tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2980
extern "C" void d2c_b2980(uint32_t u, const cl::Env& e) {
    if ((int32_t)e.H(0xc406, u) < 0x64) e.H(0xc2c8, u, 0x1f4);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
