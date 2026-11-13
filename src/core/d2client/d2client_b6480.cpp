// 1.13c D2Client+0xB6480 -- the same body as D2Client+0xB63C0 (a second copy of the compiler's output; the same two thunks and the same 0xB30F0 request).
// fastcall, ECX = a missile spawn context (+4 counter, +8 -> the request source), EDX = the unit. Returns 0 when the counter is not below source +0x10, else 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b6480(uint32_t ctx, uint32_t u, const cl::Env& e) {
    const uint32_t pa = e.R(ctx + 8);
    if ((int32_t)e.R(ctx + 4) >= (int32_t)e.R(pa + 0x10)) return 0;
    uint32_t q[23] = {};
    q[0] = 0x20;
    q[1] = e.R(pa);
    q[2] = e.R(pa + 4);
    q[4] = e.R(pa + 0x14);
    q[11] = e.R(pa + 8);
    q[12] = e.R(pa + 0xc);
    const int32_t ty = (int32_t)e.R(u);
    const uint32_t p = e.R(u + 0x2c);
    const bool direct = ty == 2 || (ty > 3 && ty <= 5);
    q[7] = direct ? e.R(p + 0xc) : (p ? e.H(0xc3be, p) : 0u);
    q[8] = direct ? e.R(p + 0x10) : (p ? e.H(0xc2ec, p) : 0u);
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    return 1;
}
