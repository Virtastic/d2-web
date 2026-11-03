// 1.13c D2Client+0x529F0 -- overlay request for a shrine-type unit: EBX = unit (null: return). Unit class [u+4] 0x21C/0x21D/0x21E selects
// the 0xC334 kind 0x1DB/0x1DC/0x1DA (any other class returns). v = 0xC334(0xC1F0(u), 2, kind), none: return. Block {0x420,u,u,.,0x21D,..,
// x@7,y@8 (read from v, the 0xC334 result, not u),..,1@12}; a non-zero result goes to 0xC448(result, [v+0xC]).
#include "cl_env.h"
extern "C" void d2c_529f0(uint32_t u, const cl::Env& e) {
    if (!u) return;
    uint32_t cls = e.R(u + 4), kind;
    if (cls == 0x21c) kind = 0x1db; else if (cls == 0x21d) kind = 0x1dc; else if (cls == 0x21e) kind = 0x1da; else return;
    uint32_t o = e.H(0xc1f0, u);
    uint32_t v = e.H(0xc334, o, 2, kind);
    if (!v) return;
    uint32_t x = e.PD(v, 0xc3be, 0xc), y = e.PD(v, 0xc2ec, 0x10);
    uint32_t q[23] = {};
    q[0] = 0x420; q[1] = u; q[2] = u; q[4] = 0x21d; q[7] = x; q[8] = y; q[12] = 1;
    uint32_t h = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (h) e.H(0xc448, h, e.R(v + 0xc));
}
