// 1.13c D2Client+0x530D0 -- overlay request 0x170 at a unit: ESI = unit. v = 0xC334(0xC1F0(u), 2, 0x182). With v: sound 0x45570(0x124F, 0, 0x15E, 0, 0, 0)
// with EBX = 0, position = v's path (x then y); without: u's own path less 15 each. Block {0x420,u,u,.,0x170,..,x@7,y@8,..,1@12}; a result and v
// together go to 0xC448(result, [v+0xC]).
#include "cl_env.h"
extern "C" void d2c_530d0(uint32_t u, const cl::Env& e) {
    uint32_t o = e.H(0xc1f0, u);
    uint32_t v = e.H(0xc334, o, 2, 0x182), x, y;
    if (v) {
        e.H(0x45570, 0x124f, 0, 0x15e, 0, 0, 0, 0, 0, 0);
        x = e.PD(v, 0xc3be, 0xc); y = e.PD(v, 0xc2ec, 0x10);
    } else { x = e.PD(u, 0xc3be, 0xc) - 15; y = e.PD(u, 0xc2ec, 0x10) - 15; }
    uint32_t q[23] = {};
    q[0] = 0x420; q[4] = 0x170; q[1] = u; q[2] = u; q[7] = x; q[8] = y; q[12] = 1;
    uint32_t h = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (h && v) e.H(0xc448, h, e.R(v + 0xc));
}
