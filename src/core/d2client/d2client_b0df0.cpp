// 1.13c D2Client+0xB0DF0 (ecx = unit): when the unit's flags (+0x44) have no 0x300 bit and its +0x64 block holds at
// least row[0x130] (bits>>3), roll the seed against row[0x131] and hand off to D2Client+0xA8BE0; otherwise return
// the flags or the block count unchanged.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0DF0
extern "C" uint32_t d2c_b0df0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    uint32_t fl = e.R(u + 0x44);
    if (fl & 0x300) return fl;
    uint32_t blk = e.R(u + 0x64);
    if (!blk) return fl;
    int32_t have = (int32_t)e.R(blk + 0x18) >> 3;
    uint32_t lo = e.R8(row + 0x130), hi = e.R8(row + 0x131);
    if (have < (int32_t)lo) return (uint32_t)have;
    uint32_t r = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, hi);                 // ecx = &seed, esi = modulus
    return e.H(0xa8be0, 0, 0, 0, 0, 0, r + lo, blk, 0, 0, 0);                         // eax, ecx = block; esi is the caller's (popped)
}
