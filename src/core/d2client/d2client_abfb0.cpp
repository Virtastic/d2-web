// 1.13c D2Client+0xABFB0 -- packet handler: stores the dword at +2 at 0x1177EC when the byte at +1 is set, else at
// 0x1177FC; the dword is left in EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_abfb0(uint32_t pkt, const cl::Env& e) {
    const uint32_t v = e.R(pkt + 2);
    clt::S(e, e.R8(pkt + 1) ? 0x1177EC : 0x1177FC, v);
    return v;
}
