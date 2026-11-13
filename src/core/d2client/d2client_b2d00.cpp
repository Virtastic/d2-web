// 1.13c D2Client+0xB2D00 (ecx = unit): client missile whose row has a sound (+0x58) and a volume (+0x5C): play it unless 0xC4D2 reports the unit muted, then the common step 0xB17C0; any other unit takes 0xB1720.
#include "cl_env.h"
extern "C" void d2c_b2d00(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || (int32_t)e.R(row + 0x58) <= 0 || (int32_t)e.R(row + 0x5c) <= 0) {
        e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
        return;
    }
    if (!e.H(0xc4d2, u)) {
        int32_t v = (int32_t)e.R(row + 0x5c);
        int32_t t = (int32_t)((uint32_t)(v / 3) * 25000u);   // 32-bit wrap as the imul
        uint32_t p = (uint32_t)(t / 25);
        e.H(0x89f00, e.R(row + 0x58), p, p);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
