// 1.13c D2Client+0x536F0 -- walk the unit chain 0xCCC4([0x11C3B8]) through +0x7C; every unit whose 0xC1DE type is 8 goes to
// 0x53550 (stdcall, one argument).
#include "cl_env.h"
extern "C" void d2c_536f0(const cl::Env& e) {
    for (uint32_t n = e.H(0xccc4, e.R(0x6FBCC3B8u)); n; n = e.R(n + 0x7c))
        if (e.H(0xc1de, n) == 8) e.H(0x53550, n);
}
