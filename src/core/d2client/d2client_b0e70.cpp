// 1.13c D2Client+0xB0E70 (esi = unit): unit teardown. Releases the +0x54 block's +0x30 allocation, frees the
// +0x54 chain through D2Client+0x69700, runs the extra pair when the global at +0x11C3B8 is set, then frees +0x2c.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0E70
extern "C" void d2c_b0e70(uint32_t u, uint32_t flag, const cl::Env& e) {
    e.H(0xc3d0, u);
    e.H(0xc35e, u);
    uint32_t b = e.R(u + 0x54);
    if (b && e.R(b + 0x30)) {
        e.H(0xbf86, 0x64, 0, 0, 0, 0, 0, e.R(b + 0x30), 0xB20D5258);
        uint32_t b2 = e.R(u + 0x54);
        if (b2) e.W(b2 + 0x30, 0);
    }
    e.H(0x69700, 0, 0, 0, 0, 0, u);
    if (flag) {
        e.H(0xc47e, u);
        e.H(0xc3ca, u);
    }
    e.H(0xc2ce, 0, e.R(u + 0x2c));
    e.W(u + 0x2c, 0);
}
