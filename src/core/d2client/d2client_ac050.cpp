// 1.13c D2Client+0xAC050 -- packet handler: the player (0x11BBFC, asserted at line 0xD15) and the dword at +1 go to
// the two-word routine; its answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_ac050(uint32_t pkt, const cl::Env& e) {
    const uint32_t p = clt::G(e, 0x11BBFC), v = e.R(pkt + 1);
    if (!p) clt::assertAt(e, 0xd15);
    return e.H(0xc508, p, v);
}
