// 1.13c D2Client+0x84af0 -- item packet 0x9C-style handler (the type-4 branch of the "item add" family). EAX = packet.
// D2Common 0xCA90(pkt + 8, byte(pkt+2) - 8, 0, &buf) decodes the payload; the sub-type is the BYTE at buf + 8. Sub-type != 4: return
// (EAX still holds the player slot word D2Client+0x11BBFC it just loaded).
// Otherwise the packet id (dword pkt+4) is looked up in the client unit hash (D2Client+0x10AE08, 128 slots, chain link +0xE4, id +0xC); a
// hit is the fatal "already exists" report (0xBBF92 / 0xBBF80 / 0x3664 exit), not vectored. A miss builds the item through 0x835C0(pkt, &buf,
// 0, 0) (EBX = 0; a null result is fatal too), then cursor = D2Common 0xC7D8(player, item), D2Common 0xC202(player), 0x162D0(EAX = that result),
// 0x81AA0, whose EAX is the return. player = [*(D2Client+0x11BBFC) + 0x60].
#include "cl_env.h"
extern "C" uint32_t d2c_84af0(uint32_t pkt, const cl::Env& e) {
    uint32_t buf[6] = {};
    uint32_t bp = (uint32_t)(uintptr_t)buf;
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, bp);
    uint32_t pl = e.R(e.self + 0x60);
    if (e.R8(bp + 8) != 4) return e.self;
    uint32_t id = e.R(pkt + 4);
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) return 0;                     // fatal in retail
    uint32_t it = e.H(0x835c0, pkt, bp, 0, 0, 0, 0, 0, 0, 0);
    if (!it) return 0;                                         // fatal in retail
    e.H(0xc7d8, pl, it);
    uint32_t r = e.H(0xc202, pl);
    e.H(0x162d0, 0, 0, 0, 0, 0, r);
    return e.H(0x81aa0);
}
