// 1.13c D2Client+0x52860 -- two overlay requests for a unit: EBX = unit, no stack words. owner = 0xC1F0(u); v = 0xC334(owner, 2, 0x173).
// Position: v's path (x then y) when v is set, else u's own path less 15 each. Block 1 (ECX = {0x420,u,u,.,0x123,..,x@7,y@8,..,1@12}) always;
// block 2 ({1,u,..,0x14c,x@5,y@6,0@11,1@12}) only when u is set, and a non-zero result goes to 0xC394(result, 0).
#include "cl_env.h"
extern "C" void d2c_52860(uint32_t u, const cl::Env& e) {
    uint32_t o = e.H(0xc1f0, u);
    uint32_t v = e.H(0xc334, o, 2, 0x173), x, y;
    if (v) { x = e.PD(v, 0xc3be, 0xc); y = e.PD(v, 0xc2ec, 0x10); }
    else { x = e.PD(u, 0xc3be, 0xc) - 15; y = e.PD(u, 0xc2ec, 0x10) - 15; }
    uint32_t q[23] = {};
    q[0] = 0x420; q[1] = u; q[2] = u; q[4] = 0x123; q[7] = x; q[8] = y; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!u) return;
    uint32_t r[23] = {};
    r[0] = 1; r[1] = u; r[5] = x; r[6] = y; r[11] = 0; r[12] = 1; r[4] = 0x14c;
    uint32_t h = e.HB(0xb30f0, (uint32_t)(uintptr_t)r);
    if (h) e.H(0xc394, h, 0);
}
