// 1.13c D2Client+0xB1C00 (ecx = unit): when the 0xC4D2 answer is even, moves the unit's path to an isometric-swapped position: with x and y from the path block (types
// 2, 4, 5) or the 0xC3BE / 0xC2EC getters, a = 0xC322(unit), b = 0xC4BA(unit), it calls 0xC3EE(path, x + y - b, y - x + a) and 0xC376(path, unit, 0). Always ends by
// tail-calling 0xB17C0 with ECX = unit.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1C00
extern "C" uint32_t d2c_b1c00(uint32_t u, const cl::Env& e) {
    if (!(e.H(0xc4d2, u) & 1)) {
        const uint32_t path = e.R(u + 0x2c);
        const uint32_t x = e.PD(u, 0xc3be, 0xc), y = e.PD(u, 0xc2ec, 0x10);
        const uint32_t a = e.H(0xc322, u), b = e.H(0xc4ba, u);
        e.H(0xc3ee, path, x + y - b, y - (x - a));
        e.H(0xc376, path, u, 0);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
