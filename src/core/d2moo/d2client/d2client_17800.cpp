// 1.13c D2Client+0x17800: monster idle/approach tick for a unit in mode 1 (EAX = unit): collision probe, nearest-target search, step toward it
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17800(uint32_t unit, const cl::Env& e) {
    if (!unit) return 0;
    uint32_t mode = e.R(unit + 0x10);
    if (mode != 1) return mode;
    uint32_t y = cl17::posY(e, unit), x = cl17::posX(e, unit);
    uint32_t room = e.H(0xc1f0, unit);
    if (e.H(0xc4a8, room, x, y, 0x1c0) & 0xffff) return e.H(0x17300, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    uint32_t tgt = 0, dist = 0x7fffffffu;
    e.H(0x16c20, cl17::ptr32(&tgt), cl17::ptr32(&dist), 0, 0, 0, 0, 0, 0, 0, unit);
    if (!tgt) return tgt;
    uint32_t path = e.R(unit + 0x2c);
    e.H(0xc400, path, 0x3c01);
    if ((int32_t)dist < 6) {
        e.H(0xc400, path, 0xc01);
        uint32_t r = d2c_17500(tgt, unit, 6, 1, 0x78, e);
        if (r) return r;
        return d2c_173e0(unit, 6, 1, 0x78, e);
    }
    uint32_t roll = e.H(0x1330, 0, 0, 0, 0, 0, 100, unit + 0x20);
    if ((int32_t)roll < 0x1e) return d2c_173e0(unit, 2, 1, 0, e);
    if (e.R(unit) == 1) { uint32_t d = e.R(unit + 0x14); if (d) { e.W(d + 0x30, 5); return d + 0x30; } }
    return roll;
}
