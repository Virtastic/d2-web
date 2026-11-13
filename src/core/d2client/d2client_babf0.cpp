// 1.13c D2Client+0xBABF0 (ecx = unit, edx = flag): flag set returns 0. Otherwise 0xC1F0(unit, x, y, 3, 1) at the unit's path position; when 0xC34C of its result has a zero low word and the unit has a missile row, 0xB9740(unit, string, 3, row +0x38, 0x19, 0x4B). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_babf0(uint32_t u, uint32_t flag, const cl::Env& e) {
    if (flag) return 0;
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    uint32_t one = 1;
    uint32_t r = e.H(0xc1f0, u, x, y, 3, 1);
    e.Xw(0xc1f0, &one, 1);
    if ((e.H(0xc34c, r) & 0xffffu) == 0) {
        uint32_t row = e.UnitRow(u);
        if (row) {
            uint32_t w[2] = { 0x19, 0x4b };
            e.H(0xb9740, u, 0xB20DD848u, 3, e.R(row + 0x38), 0x19);
            e.Xw(0xb9740, w, 2);
        }
    }
    return 1;
}
