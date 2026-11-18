// 1.13c D2Client+0xB24A0 (ecx = unit): on unit class 8 reset the +0x54/+0x30 block fields and set ordinal 10441 to
// half of the short at +0x4c; on class 0x23 set it to double; then tail to B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB24A0
extern "C" void d2c_b24a0(uint32_t u, const cl::Env& e) {
    uint32_t t = e.H(0xc4d2, u);
    if (t == 0x23) {
        e.H(0xc2fe, u, (uint32_t)(e.R16s(u + 0x4c) * 2));
    } else if (t == 8) {
        uint32_t b = e.R(u + 0x54);
        if (b) { uint32_t c = e.R(b + 0x30); if (c) { e.W(c + 0x1c, 0); e.W(c + 0x20, 0); e.W(c + 0x24, 0x100); } }
        e.H(0xc2fe, u, (uint32_t)(e.R16s(u + 0x4c) >> 1));
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
