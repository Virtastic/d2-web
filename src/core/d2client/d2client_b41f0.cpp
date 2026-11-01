// 1.13c D2Client+0xb41f0 -- client missile: the periodic smoke/flare spawner of a counting unit, then its tick (0xB17C0).
// ECX = unit. n = D2Common counter (0xC406); n == 0x145 also arms the screen-shake globals (0x10B9CC..0x10B9D8 and
// 0x11BEFC) from GetTickCount. The step table at 0xD3610 (thresh, divA, divB; four rows and a fallback) picks the two
// periods: n % divA == 0 spawns class 0x2a, n % divB == 0 spawns class 0x16c with a fixed rise and fall motion.
// gl: +0x40 = the four dwords at 0x10B9CC, +0x50 = the dword at 0x11BEFC.
#include "cl_env.h"
namespace {
const int32_t kStep[5][3] = { { 350, 3, 5 }, { 300, 2, 3 }, { 200, 3, 2 }, { 175, 5, 1 }, { 0, 100, 5 } };   // 0xD3610
}
extern "C" void d2c_b41f0(uint32_t u, uint32_t gl, uint32_t tick, const cl::Env& e) {
    int32_t n = (int32_t)e.H(0xc406, u);
    if (n == 0x145) {
        e.W(gl + 0x40, tick); e.W(gl + 0x44, 0); e.W(gl + 0x48, 0x1770); e.W(gl + 0x4c, 0);
        e.W(gl + 0x50, 0x14);
    }
    int i = 0;
    while (i < 4 && n <= kStep[i][0]) ++i;
    uint32_t px = e.PD(u, 0xc3be, 0xc);
    uint32_t py = e.PD(u, 0xc2ec, 0x10);
    for (int k = 0; k < 2; ++k) {
        if (n % kStep[i][1 + k] != 0) continue;
        uint32_t q[23] = {};
        q[0] = 0x2001;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[4] = k ? 0x16c : 0x2a;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        uint32_t r1 = e.Lcg(u + 0x20) % 0x28;
        uint32_t r2 = e.Lcg(u + 0x20) % 0x28;
        q[5] = px + r1 - 0x14;
        q[6] = py + r2 - 0x14;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (!k || !m) continue;
        uint32_t pa = e.R(m + 0x54);
        if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 4, 0); e.W(mo + 8, 0); e.W(mo + 0xc, 0x4b000); } }
        pa = e.R(m + 0x54);
        if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, 0xffffb000u); } }
        e.H(0xc4a2, m, 0xf);
        e.H(0xc2c8, m, 0xf);
        e.H(0xc382, m, 0x80);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
