// 1.13c D2Client+0x20A60 -- two overlay requests then a sound: ESI = unit, no stack words. x/y of the unit (path block for
// types 2/4/5, else the D2Common getters); 0xB30F0(ECX = zeroed 0x5C block {1, unit, .., +0x10 kind, +0x14 x, +0x18 y,
// +0x30 1}) for kind 0x75 then 0x221; then 0x88A70(unit, 0, 0, 0) with EBX = 0x99A.
#include "cl_env.h"
extern "C" void d2c_20a60(uint32_t u, const cl::Env& e) {
    uint32_t x = e.PD(u, 0xc3be, 0xc), y = e.PD(u, 0xc2ec, 0x10);
    static const uint32_t kinds[2] = {0x75u, 0x221u};
    for (uint32_t kind : kinds) {
        uint32_t q[23] = {};
        q[0] = 1; q[1] = u; q[5] = x; q[6] = y; q[11] = 0; q[12] = 1; q[4] = kind;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, 0x99a);
}
