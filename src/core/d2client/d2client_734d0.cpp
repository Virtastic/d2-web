// 1.13c D2Client+0x734D0 -- clear a unit's skill state. ESI = unit (plain ret).
#include "cl_env.h"
extern "C" void d2c_734d0(uint32_t u /*ESI*/, const cl::Env& e) {
    for (uint32_t i = 0; i < 0x10; i++) e.H(0x69790, 0, 0, 0, 0, 0, i, 0, u);
    if (u) {
        uint32_t k = e.R(u);
        if (k == 0) { if (e.R(u + 0x10) == 0x12) return; }
        else if (k != 1) goto tail;
        e.W(u + 0xc4, e.R(u + 0xc4) & 0xffffffbfu);
        if (e.H(0xc43c, u, 1)) {
            e.H(0x6a220, 0, 0, 0, 0, 0, u);
            e.H(0x2a590, 0, 0, 0, 0, 0, 0, 1, u);
            e.H(0x24490, 1, 0, 0, 0, 0, u);
        }
    }
tail:
    e.H(0x6a220, 1, 0, 0, 0, 0, u);
    e.H(0xc472, u);
    if (u) e.W(u + 0x44, 0);
    e.H(0xc778, u, 0);
}
