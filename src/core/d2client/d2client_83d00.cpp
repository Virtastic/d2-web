// 1.13c D2Client+0x83D00 itemPkt_replace: EAX = decoded item buffer (b), stack arg = packet (pkt, ret 4). aux = address of the retail 0x109854 dword.
#include "cl_env.h"
extern "C" void d2c_83d00(uint32_t pkt, uint32_t b, uint32_t aux, const cl::Env& e) {
    const uint32_t path = 0xB2000000u | 0xD4964;
    uint32_t pl = e.self;
    if (!pl) return;                                            // fatal 0x8A6 in retail
    uint32_t id = e.R(pkt + 4), item = 0;
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) {
            if (e.R(n) != 4) return;                            // fatal 0x5A in retail
            item = n;
            break;
        }
    uint32_t al = e.H(0xc92e, item) & 0xff;
    uint32_t inv = e.R(pl + 0x60);
    if (!inv) return;                                           // fatal 0x8AB in retail
    if (e.R(pl) == 0) e.H(0xca0c, inv);
    if (e.H(0xc1a8, item, 0x100, 0x96, path) == 0) e.H(0xc544, pl, item);
    uint32_t r = e.H(0xc6d0, inv, item);
    if (!r || r != item) return;                                // fatal 0x8B0 / 0x8B1 in retail
    e.H(0xc8a4, pl, al, e.R(aux));
    e.H(0xc9fa, path, 0x8b4, inv, e.R(b + 4) & 0xffff, e.R(b + 6) & 0xffff);
    e.H(0x8bbf0);
    if (e.H(0xca3c, e.R(b + 0xc), 1) == 0) {
        e.H(0x82980, 0, e.R8(b + 8), 0, e.R8(b + 0x10), 0, item);
    } else {
        e.H(0xa6ef0, id, 4, 0, 0, 0, 0, 0xB2000000u | 0x10ae08, id & 0x7f);
        item = e.H(0x835c0, pkt, b, 0, 0, 0, 0, 0, 0, 0);
        if (e.blk) e.blk(e.ctx, 0x835c0, b, 6);
        if (!item) return;                                      // fatal 0x8BB in retail
    }
    e.H(0xc7d8, inv, item);
    uint32_t c = e.H(0xc202, inv);
    e.H(0x162d0, 0, 0, 0, 0, 0, c);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, item);
}
