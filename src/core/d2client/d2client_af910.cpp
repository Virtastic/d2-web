// 1.13c D2Client+0xAF910: server packet relay. The first 8 bytes of the packet are copied to a stack block whose address goes to 0x86A40;
// its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_af910(uint32_t pk, const Env& e) {
    uint32_t L[2] = { e.R(pk), e.R(pk + 4) };
    const uint32_t r = e.H(0x86a40, Stk(L));
    if (e.blk) e.blk(e.ctx, 0x86a40, Stk(L), 2);
    return r;
}
