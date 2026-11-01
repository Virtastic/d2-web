// 1.13c D2Client+0x173E0: walk the unit a random +-s0 step from where it stands: ESI = unit, stack (s0 = step, s1 = ecx, s2 = block word 4), ret 0xC
#include "cl17.h"
extern "C" uint32_t d2c_173e0(uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, const cl::Env& e) {
    uint32_t x = cl17::posX(e, unit), y = cl17::posY(e, unit);
    uint32_t a = s0 & 0xff, dx = a, dy = a;
    if (cl17::lcg(e, unit) & 1) dx = (uint32_t)-(int32_t)dx;
    if (cl17::lcg(e, unit) & 1) dy = (uint32_t)-(int32_t)dy;
    return cl17::walk(e, unit, e.R(unit + 0x2c), dx + x, dy + y, s2, s1, unit);
}
