// 1.13c D2Client+0x52320 -- three requests for an id (ESI = id): 0x11C (a non-null result goes to 0xC448(r, 0x32)), then a
// fresh block 0x128, then the same block with only [4] rewritten to 0x125. Each block: 23 words, [1] = [2] = id, [12] = 1.
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_52320(uint32_t id, const cl::Env& e) {
    uint32_t q[23] = {};
    q[1] = id; q[2] = id; q[4] = 0x11c; q[12] = 1;
    uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (r) e.H(0xc448, r, 0x32);
    memset(q, 0, sizeof q); q[1] = id; q[2] = id; q[4] = 0x128; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    q[4] = 0x125;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
