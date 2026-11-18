// 1.13c D2Client+0xAF8B0: server packet relay. The first 21 bytes of the packet (five dwords and a byte) are copied to a stack block
// whose address goes to 0xAAA20; its result is returned. The block's last three bytes are never written.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_af8b0(uint32_t pk, const Env& e) {
    uint32_t L[6] = {};
    for (uint32_t i = 0; i < 5; i++) L[i] = e.R(pk + 4 * i);
    L[5] = B(pk + 20);
    const uint32_t r = e.H(0xaaa20, Stk(L));
    if (e.blk) e.blk(e.ctx, 0xaaa20, Stk(L), 6);
    return r;
}
