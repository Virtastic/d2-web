// 1.13c D2Client+0x73560 -- deactivate a unit's skill state. ECX = unit (plain ret).
#include "cl_env.h"
extern "C" void d2c_73560(uint32_t u /*ECX*/, const cl::Env& e) {
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) & 0xffffffbfu);
    if (e.H(0xc43c, u, 1)) {
        e.H(0x6a220, 0, 0, 0, 0, 0, u);
        e.H(0x2a590, 0, 0, 0, 0, 0, 0, 1, u);
        e.H(0x24490, 1, 0, 0, 0, 0, u);
    }
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x80000000u);
    e.H(0xc502, u);
}
