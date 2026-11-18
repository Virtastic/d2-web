// 1.13c D2Client+0x20B70 -- fastcall ECX = unit, one unused stack word (ret 4). 10569(unit, &kind, &other); unless the unit is
// present with a +0x10 mode other than 0 and 3, kind 2 calls 10973(unit, 0xC, 0) and hands its result (EAX) to
// 0x20340(unit, 0xE1).
#include "cl_env.h"
extern "C" void d2c_20b70(uint32_t u, const cl::Env& e) {
    uint32_t k[2] = {0, 0};
    e.H(0xc5ec, u, (uint32_t)(uintptr_t)&k[0], (uint32_t)(uintptr_t)&k[1]);
    if (u) { uint32_t m = e.R(u + 0x10); if (m != 3 && m != 0) return; }
    if (k[0] != 2) return;
    uint32_t r = e.H(0xc1c6, u, 0xc, 0);
    e.H(0x20340, u, 0xe1, 0, 0, 0, r);
}
