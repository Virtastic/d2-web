// 1.13c D2Client+0x40130 -- table entry: reseeds a unit (seed block at +0x20) with EDX and restarts its path at mode
// 10 with the unit's level capped at 0x4D. Without a unit EAX is left as it came in.
#include "cl_tbl.h"
extern "C" uint32_t d2c_40130(uint32_t eax, uint32_t u, uint32_t edx, const cl::Env& e) {
    if (!u) return eax;
    const uint32_t n = clt::cappedLevel(e, u);
    const uint32_t path = e.R(u + 0x2c);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, u + 0x20, edx);
    return clt::pathCommit(e, path, u, n);
}
