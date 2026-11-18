// 1.13c D2Client+0x17950: monster attack step: EAX = actor (EDI), ECX = target unit (ESI), stack s0 = engage flag, ret 4
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17950(uint32_t actor, uint32_t unit, uint32_t s0, const cl::Env& e) {
    uint32_t r = e.H(0xc538, unit);
    if (r) return r;
    r = e.H(0x16cb0, 0, 0, 0, 0, 0, unit);
    if (r) return e.H(0x17300, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    if (!unit) return r;
    uint32_t mode = e.R(unit + 0x10);
    if (mode != 1) return mode;
    if (s0 <= 1) return d2c_17500(actor, unit, 4, 1, 0, e);
    if ((int32_t)e.H(0x1330, 0, 0, 0, 0, 0, 100, unit + 0x20) < 0x4b) {
        cl17::setState(e, unit, 0x20);
        return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    }
    return d2c_173e0(unit, 4, 1, 0, e);
}
