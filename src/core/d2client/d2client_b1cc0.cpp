// 1.13c D2Client+0xB1CC0 (ecx = unit): when the +0x54/+0x30 block's +0xC the shifted value (>> 11) lies outside
// [-ordinal 11018, ordinal 10818] run D2Client+0xB1720, else tail to D2Client+0xB17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1CC0
extern "C" void d2c_b1cc0(uint32_t u, const cl::Env& e) {
    int32_t lo = -(int32_t)e.H(0xc322, u);
    int32_t hi = (int32_t)e.H(0xc4ba, u);
    int32_t v = 0;
    if (u) { uint32_t b = e.R(u + 0x54); if (b) { uint32_t c = e.R(b + 0x30); if (c) v = (int32_t)e.R(c + 0xc) >> 11; } }
    if (v < lo || v > hi) e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    else e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
