// 1.13c D2Client+0x18B20: ESI = unit. Clears the two words at D2Client+0x11BF58 / +0x11BF5C, runs 0xCD12(unit, 0), sets flag 0x10000 / clears 4 in unit+0xC4,
// clears bit 2 when 0xC1FC(unit, 7) is 0, calls 0x68E30 (eax = unit), zeroes the 20 byte pairs at unit+0x54's block, returns 0xC40C(unit, 6, 0, 0)
// g = host address of image bytes at RVA 0x11BF58 (words at g and g+4). A unit without a +0x54 block takes the logged error exit, never vectored.
#include "cl17.h"
extern "C" uint32_t d2c_18b20(uint32_t unit, uint32_t g, const cl::Env& e) {
    e.W(g, e.R(g) & 0xffff0000u);
    e.W(g + 4, e.R(g + 4) & 0xffff0000u);
    e.H(0xcd12, unit, 0);
    if (unit) e.W(unit + 0xc4, (e.R(unit + 0xc4) & 0xfffffffbu) | 0x10000u);
    if (!e.H(0xc1fc, unit, 7) && unit) e.W(unit + 0xc4, e.R(unit + 0xc4) & 0xfffffffdu);
    e.H(0x68e30, 0, 0, 0, 0, 0, unit);
    uint32_t blk = unit ? e.R(unit + 0x54) : 0;
    if (!blk) return 0;
    for (uint32_t i = 0; i < 0x14; i++) e.W(blk + i * 2, e.R(blk + i * 2) & 0xffffff00u);
    return e.H(0xc40c, unit, 6, 0, 0);
}
