// 1.13c D2Client+0x17C00: mode-1 type-1 unit (EAX) keeps its distance from the local player: flee step, or the 0xA6EF0 notify when already fleeing
#include "cl17.h"
extern "C" uint32_t d2c_17390(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_17c00(uint32_t unit, const cl::Env& e) {
    if (!unit) return 0;
    uint32_t mode = e.R(unit + 0x10);
    if (mode != 1) return mode;
    if (e.R(unit) != 1) return mode;
    uint32_t d = e.R(unit + 0x14);
    if (!d) return d;
    uint32_t f = d + 0x34, self = e.self;
    int32_t dist = (int32_t)e.H(0xc190, unit, self);
    if (e.R(f) != 0 && dist > 0x19) {
        uint32_t id = e.R(unit + 0xc);
        return e.H(0xa6ef0, id, 1, 0, 0, 0, 0, 0, id & 0x7f);
    }
    e.W(f, 1);
    uint32_t seedp = unit + 0x20, a, b;
    uint32_t bit = cl17::lcg(e, unit) & 1;
    if (bit) { a = 0x28; b = e.H(0x1330, 0, 0, 0, 0, 0, 0x28, seedp); }
    else { a = e.H(0x1330, 0, 0, 0, 0, 0, 0x28, seedp); b = 0x28; }
    if (cl17::lcg(e, unit) & 1) a = (uint32_t)(0 - a);
    if (cl17::lcg(e, unit) & 1) b = (uint32_t)(0 - b);
    uint32_t px = cl17::posX(e, self), py = cl17::posY(e, self);
    uint32_t qx = px + a, qy = py + b;
    uint32_t ux = cl17::posX(e, unit), uy = cl17::posY(e, unit);
    e.H(0xc3b8, e.R(unit + 0x2c), 0x5a);
    return d2c_17390(3 * qx - 2 * ux, 3 * qy - 2 * uy, unit, 1, 0, e);
}
