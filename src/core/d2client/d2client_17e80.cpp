// 1.13c D2Client+0x17E80: mode-1 unit (EAX): nearest-target search, close = step toward it, else wander or state 5
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17e80(uint32_t unit, const cl::Env& e) {
    if (!unit) return 0;
    uint32_t mode = e.R(unit + 0x10);
    if (mode != 1) return mode;
    uint32_t tgt = 0, dist = 0x7fffffffu;
    e.H(0x16c20, cl17::ptr32(&tgt), cl17::ptr32(&dist), 0, 0, 0, 0, 0, 0, 0, unit);
    if (!tgt) return tgt;
    uint32_t path = e.R(unit + 0x2c);
    e.H(0xc400, path, 0x3c01);
    if ((int32_t)dist < 4) {
        e.H(0xc400, path, 0xc01);
        uint32_t r = d2c_17500(tgt, unit, 4, 0xc, 0x78, e);
        if (r) return r;
        return d2c_173e0(unit, 4, 0xc, 0x78, e);
    }
    uint32_t lo = cl17::lcg(e, unit);
    if (lo % 100 < 0x1e) return d2c_173e0(unit, 2, 1, 0, e);
    if (e.R(unit) == 1) { uint32_t d = e.R(unit + 0x14); if (d) { e.W(d + 0x30, 5); return d + 0x30; } }
    return lo / 100;
}
