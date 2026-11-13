// 1.13c D2Client+0xBACB0 (ecx = unit, edx = owner): with a +0x2C child class on the unit's row and a room from 0xA60D0, run 0xB6290 (eax = owner, or the unit when 0) with the class and max(row +0x6C, 1). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_bacb0(uint32_t u, uint32_t owner, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    if (!owner) owner = u;
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n < 1) n = 1;
    uint32_t a = e.H(0xc3fa, u), b = e.H(0xc388, u);
    e.H(0xb6290, room, b, a, (uint32_t)n, 0, owner, 0, (uint32_t)e.R16s(row + 0x2c));
    return 1;
}
