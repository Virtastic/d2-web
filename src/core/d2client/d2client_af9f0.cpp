// 1.13c D2Client+0xAF9F0: server packet carrying two strings. A 0x11C-byte stack block takes the packet's first ten bytes, the first string
// (0xBE96 copy of at most 0x10 bytes from +10 into block+10), and the second string (0xBE90 measures the first; 0xBE96 copies at most
// 0x100 bytes from just past its terminator into block+0x1A); 0x7F780 gets the block and its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_af9f0(uint32_t pk, const Env& e) {
    uint32_t L[0x47] = {};
    uint8_t* b = (uint8_t*)L;
    for (uint32_t i = 0; i < 10; i++) b[i] = (uint8_t)B(pk + i);
    e.H(0xbe96, Stk(b + 10), pk + 10, 0x10);
    const uint32_t len = e.H(0xbe90, pk + 10);
    e.H(0xbe96, Stk(b + 0x1a), pk + 10 + len + 1, 0x100);
    const uint32_t r = e.H(0x7f780, Stk(L));
    if (e.blk) e.blk(e.ctx, 0x7f780, Stk(L), 3);
    return r;
}
