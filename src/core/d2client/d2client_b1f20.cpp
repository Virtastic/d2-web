// 1.13c D2Client+0xB1F20 (ecx = unit): the 0xB1C00 body without its 0xC4D2 test: 0xC3EE(path, x + y - b, y - x + a), 0xC376(path, unit, 0), then a tail call to 0xB17C0
// with ECX = unit.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1F20
extern "C" uint32_t d2c_b1f20(uint32_t u, const cl::Env& e) {
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t x = e.PD(u, 0xc3be, 0xc), y = e.PD(u, 0xc2ec, 0x10);
    const uint32_t a = e.H(0xc322, u), b = e.H(0xc4ba, u);
    e.H(0xc3ee, path, x + y - b, y - (x - a));
    e.H(0xc376, path, u, 0);
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
