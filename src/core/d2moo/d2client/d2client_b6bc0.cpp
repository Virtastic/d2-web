// 1.13c D2Client+0xb6bc0 -- client missile: two 16-point rings (offsets +-2) of children of Missiles class `cls`
// around unit u. EAX = u, EBX = cls, EDX = p, stack s1, s2 (ret 8); no return value (EAX is left over).
// cls == 0 does nothing; any other class must be a valid row (retail reads row +0x38 unchecked). Request type 0x17,
// or 0x1F with block word 13 = p when p > 0. Pass 1: every max(s2, 1)-th point of 16, word 10 = row +0x38 << 7,
// words 7/8 from the tables at 0xD3DB0 / 0xD3D70. Pass 2 (s1 > 0): every s1-th of the first 15 points shifted
// by one, word 10 = row +0x3C << 7.
#include "cl_env.h"
namespace {
const int32_t kY[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };   // 0xD3D70
const int32_t kX[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };   // 0xD3DB0
}
extern "C" void d2c_b6bc0(uint32_t u, int32_t cls, int32_t p, int32_t s1, int32_t s2, const cl::Env& e) {
    uint32_t row = e.Row(cls);
    if (!cls) return;
    uint32_t q[23] = {};
    q[0] = 0x17;
    if (p > 0) { q[0] = 0x1f; q[13] = (uint32_t)p; }
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[2] = u; q[4] = (uint32_t)cls;
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[10] = e.R(row + 0x38) << 7;
    if (s2 < 1) s2 = 1;
    for (int32_t i = 0; i < 16; i += s2) {
        q[7] = (uint32_t)kX[i]; q[8] = (uint32_t)kY[i];
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    if (s1 <= 0) return;
    q[10] = e.R(row + 0x3c) << 7;
    for (int32_t i = 0; i < 15; i += s1) {
        q[7] = (uint32_t)kX[i + 1]; q[8] = (uint32_t)kY[i + 1];
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
}
