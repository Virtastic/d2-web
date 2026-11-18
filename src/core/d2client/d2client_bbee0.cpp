// 1.13c D2Client+0xBBEE0 (ecx = unit, edx = flag): flag set returns 0; otherwise two 0xA27B0 / 0xB0450 effect calls (ids 0x16D, 0x16E) at the unit's two coordinates. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_bbee0(uint32_t u, uint32_t flag, const cl::Env& e) {
    if (flag) return 0;
    uint32_t a = e.H(0xc3fa, u), b = e.H(0xc388, u);
    e.Xs(a);   // sixth stack word (the log keeps the first four)
    e.H(0xa27b0, u, 0x16d, 3, 6, b);
    a = e.H(0xc3fa, u); b = e.H(0xc388, u);
    e.H(0xb0450, 0x16e, 0, 0, b, a, 0, 0, 0, 0, u);
    return 1;
}
