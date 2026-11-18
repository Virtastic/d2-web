// 1.13c D2Client+0x17390: walk the unit to (x, y): EAX = x, ECX = y, EDI = unit, stack (action ecx, s4), ret 8
#include "cl17.h"
extern "C" uint32_t d2c_17390(uint32_t x, uint32_t y, uint32_t unit, uint32_t s0, uint32_t s1, const cl::Env& e) {
    return cl17::walk(e, unit, e.R(unit + 0x2c), x, y, s1, s0, unit);
}
