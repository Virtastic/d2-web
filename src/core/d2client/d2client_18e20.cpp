// 1.13c D2Client+0x18E20: EAX = unit, stack a0..a2 (ret 0xc). Refreshes a unit's state packet for modes 2, 3 and 6: asks 0xCF10, then dispatches 0x19830 with the shape's pair
#include "cl17.h"
extern "C" uint32_t d2c_18e20(uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2, const cl::Env& e) {
    int32_t mode = (int32_t)e.R(unit + 0x10);
    uint32_t l1c = e.R(unit + 0x2c), l18 = 0, l14 = 0, flag = 0, shape = 0;
    if (mode >= 2 && (mode <= 3 || mode == 6)) {
        flag = 1;
        e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
        shape = e.H(0xc2da, e.R(unit + 0x2c));
        if (!shape) e.H(0xa1340, cl17::ptr32(&l18));
    }
    uint32_t r = e.H(0xcf10, l1c, unit, a0, a1, a2);
    if (!flag) return r;
    uint32_t m[7] = { 0, 0, 0, 0, 0, 0, 0 }, c;
    if (shape) { m[0] = e.R(shape); m[1] = e.R(shape + 0xc); c = mode == 3 ? 0x18 : 0; }
    else { m[0] = l14; m[1] = l18; c = mode == 3 ? 0x17 : 1; }
    return e.H(0x19830, cl17::ptr32(m), 0, 0, 0, 0, 0, c, unit);
}
