// 1.13c D2Client+0xB0100 (esi = unit): 3 unless the unit's table row carries gdwBitMasks[2] and stat 0x148 is nonzero
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0100
extern "C" uint32_t d2c_b0100(uint32_t u, const cl::Env& e) {
    uint32_t r = 3;
    uint32_t row = e.Row((int32_t)e.H(0xc37c, u));                                // Ordinal 10191(unit) ret 4
    if (!(e.R8(e.bm + 8) & e.R8(row + 4))) return 3;
    uint32_t n = e.H(0xc1c6, u, 0x148, 0);                                       // stat get (unit, 0x148, 0)
    if (n) { e.H(0xc40c, u, 0x148, n - 1, 0); r = 2; }                           // stat set (unit, 0x148, n-1, 0)
    return r;
}
