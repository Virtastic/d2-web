// 1.13c D2Client+0x53070 -- sound 0x1253 then an overlay request: ESI = unit. 0x45570(0x1253, 0, 0xFA, 0, 0, 0) with EBX = 0; block
// {.,u,u,.,0x17B,..,1@12} (all else zero) through 0xB30F0. Results ignored.
#include "cl_env.h"
extern "C" void d2c_53070(uint32_t u, const cl::Env& e) {
    e.H(0x45570, 0x1253, 0, 0xfa, 0, 0, 0, 0, 0, 0);
    uint32_t q[23] = {};
    q[1] = u; q[2] = u; q[4] = 0x17b; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
