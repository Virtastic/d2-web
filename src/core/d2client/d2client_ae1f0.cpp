// 1.13c D2Client+0xAE1F0: server packet relay. The first 18 bytes of the packet are copied to a stack block (four dwords and a word),
// whose address goes to 0x84DF0 in EBX; its result is returned. The block's last two bytes are never written.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ae1f0(uint32_t pk, const Env& e) {
    uint32_t L[5] = {};
    for (uint32_t i = 0; i < 4; i++) L[i] = e.R(pk + 4 * i);
    L[4] = W16(pk + 16);
    const uint32_t r = e.H(0x84df0, 0, 0, 0, 0, 0, 0, 0, 0, Stk(L));
    if (e.blk) e.blk(e.ctx, 0x84df0, Stk(L), 5);
    return r;
}
