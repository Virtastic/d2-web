// 1.13c D2Client+0x7BEB0 (stdcall, one argument): chat line queue {head u32, count u16}. Walk the chain (node+0x22 is the next link, read before the
// frees), free each node's six strings (line 0x6af) and the node (line 0x6b2), count the nodes in a 16 bit counter, clear the head, and require the count to
// equal the stored u16: a mismatch is the logged fatal exit (0xBF92 message 0x6c9, 0xBF80 print, 0x3664 exit(-1)), which does not return in retail.
// The stored count is then cleared as a 16 bit store.
#include "cl_env.h"
extern "C" void d2c_7beb0(uint32_t q, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4BDCu;
    uint16_t n = 0;
    uint32_t node = e.R(q);
    if (node) {
        do {
            uint32_t nxt = e.R(node + 0x22);
            for (uint32_t i = 0; i < 6; i++) { uint32_t p = e.R(node + i * 4); if (p) e.H(0xbf86, 0x6af, 0, 0, 0, 0, 0, p, kFile); }
            e.H(0xbf86, 0x6b2, 0, 0, 0, 0, 0, node, kFile);
            n++;
            node = nxt;
        } while (node);
        e.W(q, 0);
    }
    if (n != (uint16_t)e.R(q + 4)) {
        uint32_t m = e.H(0xbf92, 0x6c9);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
        return;   // exit(-1) in retail
    }
    e.W(q + 4, e.R(q + 4) & 0xffff0000u);
}
