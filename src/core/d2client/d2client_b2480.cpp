// 1.13c D2Client+0xB2480 (ecx = unit): clear the +0x44 flags when D2Client+0xA60D0 accepts the unit, then tail to B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2480
extern "C" void d2c_b2480(uint32_t u, const cl::Env& e) {
    if (e.H(0xa60d0, 0, 0, 0, 0, 0, u)) e.W(u + 0x44, 0);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
