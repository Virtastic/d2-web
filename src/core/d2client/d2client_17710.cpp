// 1.13c D2Client+0x17710: idle choice for a mode-1 monster: EAX = unit (ESI), EBX = target unit (may be 0)
#include "cl17.h"
extern "C" uint32_t d2c_176a0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17710(uint32_t unit, uint32_t tgt, const cl::Env& e) {
    uint32_t mode = unit ? e.R(unit + 0x10) : 0;
    uint32_t r = e.H(0xc538, unit);
    if (r != 0 || mode != 1) return r;
    auto hold = [&]() {   // set the data block's state word to 0x14 (unit type 1 only), then the 0x17270 walk
        if (unit && e.R(unit) == 1) { uint32_t d = e.R(unit + 0x14); if (d) e.W(d + 0x30, 0x14); }
        return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    };
    if (e.H(0x1c10, 0, 0, 0, 0, 0, unit ? e.R(unit + 4) : 0xffffffffu, 0, 2) == 0) return hold();
    uint32_t rem = cl17::lcg(e, unit) % 100;
    if ((int32_t)rem < 0x28) return hold();
    if ((int32_t)rem < 0x3c && tgt) return d2c_176a0(tgt, unit, 0, 6, e);
    if ((int32_t)rem < 0x50 && tgt) return d2c_17500(tgt, unit, 8, 1, 4, e);
    return d2c_173e0(unit, 8, 1, 4, e);
}
