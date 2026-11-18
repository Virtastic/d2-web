// 1.13c D2Client+0x839E0 itemPkt_toCursor: EAX = packet. hud = address of the retail 0x11B830 block (cursor globals at +4, +0x24, +0x28, +0x34).
#include "cl_env.h"
// Returns the EAX retail leaves: the cursor item 0xC202 hands back (the fatal exits are not vectored).
extern "C" uint32_t d2c_839e0(uint32_t pkt, uint32_t hud, const cl::Env& e) {
    uint32_t buf[6] = {};
    uint32_t bp = (uint32_t)(uintptr_t)buf;
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, bp);
    if (!e.self) return 0;                                      // fatal 0xB0C in retail
    uint32_t id = e.R(pkt + 4);
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) {
            if (e.R(n) != 4) return 0;                          // fatal 0x5A in retail
            e.H(0xa6ef0, id, 4, 0, 0, 0, 0, 0xB2000000u | 0x10ae08, id & 0x7f);
            break;
        }
    uint32_t it = e.H(0x835c0, pkt, bp, 0, 0, 0, 0, 0, 0, 0);
    if (e.blk) e.blk(e.ctx, 0x835c0, bp, 6);
    uint32_t inv = e.R(e.self + 0x60);
    if (!inv) return 0;                                         // fatal 0xB13 in retail
    e.H(0xc7d8, inv, it);
    uint32_t c = e.H(0xc202, inv);
    e.W(hud + 4, c);
    e.W(hud + 0x28, 0);
    e.W(hud + 0x24, 5);
    e.W(hud + 0x34, c ? 4 : 1);
    return c;
}
