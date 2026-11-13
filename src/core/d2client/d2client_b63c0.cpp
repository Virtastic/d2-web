// 1.13c D2Client+0xB63C0 -- fastcall, ECX = a missile spawn context (+4 counter, +8 -> the request source), EDX = the unit.
// pa = ctx +8; a counter (ctx +4) not below pa +0x10 (signed) returns 0. Otherwise builds a 23-dword zeroed request on the stack and calls 0xB30F0 (ECX = request):
// word 0 = 0x20, 1 = pa +0, 2 = pa +4, 4 = pa +0x14, 11 = pa +8, 12 = pa +0xC; words 7/8 = the unit's path block +0xC/+0x10 when its type is 2 or in 4..5,
// else 0 for a null path and 0xC3BE(path) / 0xC2EC(path). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b63c0(uint32_t ctx, uint32_t u, const cl::Env& e) {
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
