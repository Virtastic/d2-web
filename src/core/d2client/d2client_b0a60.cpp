// 1.13c D2Client+0xB0A60 (eax = unit, stack: other, ret 4): a hero-or-nothing unit whose stats 0x9c+0xa6 sum to a
// percentage rolls up to four times against its LCG; each success raises stat 0x148 on `other` by one.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0A60
extern "C" void d2c_b0a60(uint32_t u, uint32_t other, const cl::Env& e) {
    if (u) { uint32_t t = e.R(u); if (t != 0 && t != 1) return; }
    uint32_t row = e.Row((int32_t)e.H(0xc37c, other));
    if (!(e.R8(e.bm + 8) & e.R8(row + 4))) return;
    uint32_t sum = e.H(0xc454, u, 0x9c, 0) + e.H(0xc1c6, u, 0xa6, 0);
    if (!sum) return;
    uint32_t chance = sum, seedLo;
    if (u && e.R(u) == 0 && e.self != u) { chance = 100; seedLo = 0; }
    else seedLo = e.H(0xc250, u, 0x148, 0);
    uint32_t lo = seedLo, hi = 0x29a;                                            // D2Common #10614: {n, 0x29A}
    uint32_t n = 0;
    while (n < 4) {
        uint64_t pr = (uint64_t)lo * 0x6ac690c5u + hi;
        lo = (uint32_t)pr; hi = (uint32_t)(pr >> 32);
        if ((int32_t)(lo % 100) >= (int32_t)chance) break;
        n++;
    }
    e.H(0xc40c, other, 0x148, n, 0);
}
