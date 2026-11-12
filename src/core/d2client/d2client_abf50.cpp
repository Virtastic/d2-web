// 1.13c D2Client+0xABF50 -- packet handler: the packet's first byte must be 0x97 (asserted, line 0xF46). When the
// check call answers nonzero and the word at 0x119854 is set, the toggle at 0x11BC94 flips between 0 and 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_abf50(uint32_t pkt, const cl::Env& e) {
    if (e.R8(pkt) != 0x97) clt::assertAt(e, 0xf46);
    if (!e.H(0xbf8c)) return 0;
    if (!clt::G(e, 0x119854)) return 0;
    const uint32_t v = clt::G(e, 0x11BC94) == 0;
    clt::S(e, 0x11BC94, v);
    return v;
}
