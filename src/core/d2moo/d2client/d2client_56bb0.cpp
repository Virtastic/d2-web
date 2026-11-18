// 1.13c D2Client+0x56BB0 -- reset a unit's cast flags and replay its stand position (ECX = unit; ret 4, the stack word unused).
// Clears bits 1..3 of unit+0xC4, takes x (static types 2/4/5: path+0x10, others 0xC2EC (path), no path 0) and y (path+0xC /
// 0xC3BE), then 0xCB98 (0xC1F0 (unit), y, x, 5, 0x1000); 1. A null unit faults in retail (the type read is unguarded).
#include "cl_env.h"
extern "C" uint32_t d2c_56bb0(uint32_t unit, const cl::Env& e) {
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) & 0xfffffff1u);
    uint32_t x = e.PD(unit, 0xc2ec, 0x10), y = e.PD(unit, 0xc3be, 0xc);
    uint32_t r = e.H(0xc1f0, unit);
    e.H(0xcb98, r, y, x, 5, 0x1000);
    return 1;
}
