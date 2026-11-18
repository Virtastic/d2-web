// 1.13c D2Client+0xB2500 (ecx = unit, void): unit class 0x96 stamps the tick and a fixed record into five module
// globals (g[0..4] = +0x10B9CC, B9D0, B9D4, B9D8, +0x11BEFC); class 0x32 calls D2Client+0x88A70 (ebx = 0x121E);
// every path ends in the tail call to B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2500
extern "C" void d2c_b2500(uint32_t u, uint32_t tick, uint32_t g, const cl::Env& e) {
    uint32_t t = e.H(0xc406, u);
    if (t == 0x96) {
        e.W(g + 0, tick); e.W(g + 4, 0); e.W(g + 8, 0xfa0); e.W(g + 12, 0); e.W(g + 16, 0x19);
    } else if (t == 0x32) {
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0x121e, 0);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
