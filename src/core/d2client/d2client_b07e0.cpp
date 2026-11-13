// 1.13c D2Client+0xB07E0 (ecx = unit): sets bit 0x40000 of the 0xA60D0 record's flag dword (+0xC8) when there is one. Returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB07E0
extern "C" uint32_t d2c_b07e0(uint32_t u, const cl::Env& e) {
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (p) e.W(p + 0xc8, e.R(p + 0xc8) | 0x40000u);
    return 1;
}
