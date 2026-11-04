// 1.13c D2Client+0x68E30 (EAX = unit): the unit's client block is unit+0x54 (null unit or null block: logged fatal exit 0x83d, not returning).
// Walk the singly linked list at block+0x28 (next at node+0x10); a node whose type word (node+0) is neither 0 nor 0x11 is freed (Fog free, line 0x847)
// and unlinked: the head is rewritten when no node has been kept yet, otherwise the last kept node's next. The kept-node pointer only advances on a keep.
#include "cl_env.h"
extern "C" void d2c_68e30(uint32_t unit, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4FA4u;
    uint32_t blk = unit ? e.R(unit + 0x54) : 0;
    if (!blk) {
        uint32_t m = e.H(0xbf92, 0x83d);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    uint32_t prev = 0, n = e.R(blk + 0x28);
    while (n) {
        uint32_t t = e.R(n), nxt = e.R(n + 0x10);
        if (t != 0 && t != 0x11) {
            e.H(0xbf86, 0x847, 0, 0, 0, 0, 0, n, kFile);
            if (!prev) e.W(blk + 0x28, nxt); else e.W(prev + 0x10, nxt);
        } else prev = n;
        n = nxt;
    }
}
