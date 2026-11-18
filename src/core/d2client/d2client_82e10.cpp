// 1.13c D2Client+0x82E10 -- item packet handler (ESI = the packet): sets up a reader over the packet body (0xCA90 import: body = packet+8,
// length = byte[packet+2] - 8, 0, a 24-byte block on the stack). The unit id at packet+4 is looked up in the 128-bucket unit table at
// 0x10AE08 (bucket = id & 0x7F, chain link at +0xE4, id at +0xC); an unknown id ends the call (EAX 0). A found unit must be of type 4 (else
// the assertion, line 0x5A). Its mode word at +0x10 selects the handler: 2 runs 0x82A00 (EAX = the packet, stack = the block), 4 runs
// 0x82B20 (stack = the packet, the block); any other mode leaves the mode in EAX.
#include "cl_pa4.h"
extern "C" uint32_t d2c_82e10(uint32_t pkt, const cl::Env& e) {
    using namespace pa4;
    uint32_t blk[6] = {};
    const uint32_t b = (uint32_t)(uintptr_t)blk;
    e.H(0xca90, pkt + 8, (e.R(pkt) >> 16 & 0xffu) - 8, 0, b);
    const uint32_t id = e.R(pkt + 4);
    uint32_t u = e.R(clc::kBase + 0x10ae08 + (id & 0x7f) * 4);
    while (u && e.R(u + 0xc) != id) u = e.R(u + 0xe4);
    if (!u) return 0;
    if (e.R(u) != 4) {
        assertAt(e, 0x5a);
        return 0;
    }
    const uint32_t m = e.R(u + 0x10);
    if (m == 2) return e.H(0x82a00, b, 0, 0, 0, 0, pkt);
    if (m == 4) return e.H(0x82b20, pkt, b);
    return m;
}
