// 1.13c D2Client+0xb6290 -- client missile: an 8-point ring (offsets +-2) of type-0x1F children of Missiles class
// `cls` around unit u. EAX = u, EDX = cls, stack a0..a3 (ret 0x10). Returns 0 for an out-of-range class, else 1.
// Block: word 1 = a0, 5/6 = unit position, 10 = row +0x38 << 7, 11 = a1, 12 = a2, 13 = a2 - 1; the ring walks
// every a3-th point of the tables at 0xD3BB8 (word 8) / 0xD3BD8 (word 7).
#include "cl_env.h"
namespace {
const int32_t kA[8] = { 2, 2, 0, -2, -2, -2, 0, 2 };   // 0xD3BB8
const int32_t kB[8] = { 0, 2, 2, 2, 0, -2, -2, -2 };   // 0xD3BD8
}
extern "C" uint32_t d2c_b6290(uint32_t u, int32_t cls, uint32_t a0, uint32_t a1, uint32_t a2, int32_t a3, const cl::Env& e) {
    uint32_t row = e.Row(cls);
    if (!row) return 0;
    uint32_t q[23] = {};
    q[0] = 0x1f; q[1] = a0; q[2] = u; q[4] = (uint32_t)cls;
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[10] = e.R(row + 0x38) << 7;
    q[11] = a1; q[12] = a2; q[13] = a2 - 1;
    for (int32_t i = 0; i < 0x20; i += a3 * 4) {
        q[8] = (uint32_t)kA[i / 4]; q[7] = (uint32_t)kB[i / 4];
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return 1;
}
