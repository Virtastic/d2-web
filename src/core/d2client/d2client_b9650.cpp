// 1.13c D2Client+0xb9650 -- client missile: spawn at the unit with flags. ESI = unit, EBX = extra, stack class, wide
// (ret 8); returns the 0xB30F0 result, 0 when 0xC352(path +0x2C) == 0. Request word 0 = 5 (0x8005 with word 19 = EBX
// when EBX > 0; bit 8 when wide > 0), 1 = 0xA60D0(EAX = u), 2 = u, 4 = class, 5/6 = the unit path +0xC/+0x10
// (static-path types) or 0xC3BE/0xC2EC(path), 11 = 0xC388(u), 12 = 0xC3FA(u), 13 = 2*word 12 - 2; 0xB30F0(ECX = block).
#include "cl_env.h"
extern "C" uint32_t d2c_b9650(uint32_t u, int32_t extra, uint32_t cls, int32_t wide, const cl::Env& e) {
    if (!e.H(0xc352, e.R(u + 0x2c))) return 0;
    uint32_t q[23] = {};
    q[0] = 5;
    if (extra > 0) { q[19] = (uint32_t)extra; q[0] = 0x8005; }
    if (wide > 0) q[0] |= 8;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[2] = u;
    q[4] = cls;
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[11] = e.H(0xc388, u);
    uint32_t v = e.H(0xc3fa, u);
    q[13] = v * 2 - 2;
    q[12] = v;
    return e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
