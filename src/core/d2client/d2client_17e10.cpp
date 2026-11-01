// 1.13c D2Client+0x17E10: EAX = unit, s0 = actor unit, s1 = range, ret 8: 66% step toward the actor, else step away
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17e10(uint32_t unit, uint32_t s0, uint32_t s1, const cl::Env& e) {
    if (!unit) return 0;
    uint32_t mode = e.R(unit + 0x10);
    if (mode != 1) return mode;
    if (s1 >= 5) return d2c_173e0(unit, 9, 1, 0, e);
    uint32_t q = cl17::lcg(e, unit) % 100;
    if (q < 0x42) return d2c_17500(s0, unit, 8, 1, 0x1e, e);
    return d2c_173e0(unit, 0xc, 1, 0x1e, e);
}
