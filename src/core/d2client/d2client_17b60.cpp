// 1.13c D2Client+0x17B60: monster idle tick (EAX = unit, one stack word unused, ret 4): 40% state 0x14 + 0x17270, else a step
#include "cl17.h"
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17b60(uint32_t unit, uint32_t, const cl::Env& e) {
    uint32_t mode = unit ? e.R(unit + 0x10) : 0;
    uint32_t r = e.H(0xc538, unit);
    if (r) return r;
    r = e.H(0x16cb0, 0, 0, 0, 0, 0, unit);
    if (r) return e.H(0x17300, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    if (mode != 1) return r;
    uint32_t q = cl17::lcg(e, unit) % 100;
    if (q < 0x28) { cl17::setState(e, unit, 0x14); return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit); }
    return d2c_173e0(unit, 4, 1, 0, e);
}
