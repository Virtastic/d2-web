// 1.13c D2Client+0xAE9C0: server packet {?, dword at +1, dword at +5, byte at +9}. When the global 0x11C3B8 equals the hero's dword at +0x1C,
// 0xC556(global, 0xC1F0(hero), dword1, dword5, byte9) and 0xA6680 run, the hero is reloaded and its unit (type byte, id at +0xC) comes
// from 0xA4E20 (EAX = id & 0x7F, ECX = id, EDX = the per-type hash table 0x10A608 + type * 0x200); 0x82FE0(unit) is the result, 0 when
// the hero or the unit is missing. When they differ the hero is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ae9c0(uint32_t pk, const Env& e, const GEnv& g) {
    uint32_t pl = e.self;
    const uint32_t cmp = e.R(pl + 0x1c), arg5 = e.R(pk + 5), arg1 = e.R(pk + 1);
    const uint32_t gl = e.R(g.G(0x11c3b8));
    if (gl != cmp) return pl;
    const uint32_t x = pl ? e.H(0xc1f0, pl) : 0;
    const uint32_t w4 = B(pk + 9);
    e.H(0xc556, gl, x, arg1, arg5, w4);
    e.Xw(0xc556, &w4, 1);
    e.H(0xa6680);
    pl = e.self;
    if (!pl) return 0;
    const uint32_t ty = e.R8(pl), id = e.R(pl + 0xc);
    const uint32_t u = e.H(0xa4e20, ty, 0, 0, 0, 0, id & 0x7f, id, g.M(0x10a608 + ty * 0x200));
    if (!u) return 0;
    return e.H(0x82fe0, u);
}
