// 1.13c D2Client+0x56D00 -- table entry: reseeds a unit (seed block at +0x20) with the path's value plus EDX, steps
// the seed once and, when the roll modulo 100 is at least 20, restarts the path at mode 10 with the level capped at
// 0x4D; otherwise the roll divided by 100 is left in EAX. Without a unit EAX is left as it came in.
#include "cl_tbl.h"
extern "C" uint32_t d2c_56d00(uint32_t eax, uint32_t u, uint32_t edx, const cl::Env& e) {
    if (!u) return eax;
    const uint32_t n = clt::cappedLevel(e, u);
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t r = e.H(0xc31c, path);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, u + 0x20, r + edx);
    const uint32_t lo = e.Lcg(u + 0x20);
    if ((int32_t)(lo % 100) < 0x14) return lo / 100;
    return clt::pathCommit(e, path, u, n);
}
