// 1.13c D2Client+0xB5300 (ecx = unit): spawn a type-0x177 child from the unit's room; when 0xB30F0 makes one, hand it 0xC448(child, 0xC322(unit)). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b5300(uint32_t u, const cl::Env& e) {
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[1] = room; q[2] = u; q[4] = 0x177;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    uint32_t c = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (c) e.H(0xc448, c, e.H(0xc322, u));
    return 1;
}
