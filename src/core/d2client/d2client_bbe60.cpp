// 1.13c D2Client+0xbbe60 -- missile callback: owner sound cue. ECX = unit, no return.
// When 10203(path) is set and the unit's Missiles row is valid with s16 +0x1e >= 0: m = 11029(u), p = 0xA60D0(EAX = u),
// v = 10231(u, p, row+0x84, class, m), then 0xB9650(ESI = u, EBX = 0; s16 +0x1e, v). Always tails into 0xB17C0(ECX = u).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bbe60(uint32_t u, const cl::Env& e) {
    if (e.H(0xc352, e.R(u + 0x2c))) {
        int32_t cls = (int32_t)e.R(u + 4);
        uint32_t row = e.Row(cls);
        if (row && e.R16s(row + 0x1e) >= 0) {
            uint32_t m = e.H(0xc3fa, u);
            uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
            uint32_t v = e.H(0xc2c2, u, p, e.R(row + 0x84), (uint32_t)cls, m);
            e.H(0xb9650, (uint32_t)e.R16s(row + 0x1e), v, 0, 0, 0, 0, 0, 0, 0, u);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
