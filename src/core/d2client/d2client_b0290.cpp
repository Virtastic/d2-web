// 1.13c D2Client+0xB0290 -- table entry: reseeds a unit (seed block at +0x20) with the path's value and restarts the
// path at mode 10 with the unit's level capped at 0xFF. Without a unit EAX is left as it came in.
#include "cl_tbl.h"
extern "C" uint32_t d2c_b0290(uint32_t eax, uint32_t u, const cl::Env& e) {
    if (!u) return eax;
    const uint32_t n = e.H(0xc430, u);
    const uint32_t lvl = (int32_t)n > 0xff ? 0xffu : n;
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t r = e.H(0xc31c, path);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, u + 0x20, r);
    return clt::pathCommit(e, path, u, lvl);
}
