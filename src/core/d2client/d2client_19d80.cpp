// 1.13c D2Client+0x19D80: EBX = unit. Local player only (otherwise returns playerSelf): after the 0xC1C6 gate, re-dispatches the unit's state through 0x19830, or settles its animation through 0xA8630 / 0x19710
#include "cl17.h"
extern "C" uint32_t d2c_19d80(uint32_t unit, const cl::Env& e) {
    if (unit != e.self) return e.self;
    uint32_t g = e.H(0xc1c6, unit, 0xa, 0);
    if (g) return g;
    uint32_t l[7] = { 0, 0, 0, 0, 0, 0, 0 };
    uint32_t esi = e.R(unit + 0x2c);
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    uint32_t edi = e.H(0xc2da, e.R(unit + 0x2c));
    if (e.H(0xc2da, esi)) {
        l[0] = edi ? e.R(edi) : 6;
        l[1] = edi ? e.R(edi + 0xc) : 0xffffffffu;
        return e.H(0x19830, cl17::ptr32(l), 0, 0, 0, 0, 0, 0, unit);
    }
    l[0] = e.H(0xc31c, esi);
    l[1] = e.H(0xc340, esi);
    uint32_t x = e.H(0xc5c2, unit, 0);
    if (x) e.H(0xa8630, 0, 0, 0, 0, 0, 0, 0, 0, 0, x);
    e.H(0x197d0, unit == e.self, 0, 0, 0, 0, unit);
    e.H(0xc778, unit, 0);
    if (e.R(unit + 0x10) == 0x13) return e.R(unit + 0x2c);
    e.H(0xcd78, e.R(unit + 0x2c));
    uint32_t ebp = e.H(0xc208, e.H(0xc1f0, unit)) ? 5 : 1;
    uint32_t rr = e.H(0x2a040, 0, 0, 0, 0, 0, l[1], l[0], 0, 0, unit);
    if (rr) return e.H(0x2a6c0, 0, 0, 0, 0, 0, rr, 0, 0, 0, unit);
    return e.H(0x19710, 1, 0, 0, 0, 0, ebp, unit);
}
