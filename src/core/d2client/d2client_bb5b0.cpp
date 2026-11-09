// 1.13c D2Client+0xbb5b0 -- missile trail spawn. stdcall(p, u, cls, f, t), ret 0x14, no meaningful return.
// Missiles row cls out of range: return. s16 row+0x1e > 0: 0xB7DE0(EAX = t, ESI = u; p, s16 +0x1e).
// s16 row+0x34 > 0 and f == 0x13B: 0x88A70(EBX = s16 +0x34; p, 0, 0, 0).
// f odd, or t < 3 and f % 16 != 0: return. n = t<3:1, t<6:2, t<9:3, else 4; for n > 1 the jitter is
// dx = 0x1380(ECX = u+0x20, ESI = n) - n/2, then dy likewise. The 0x5C-byte request block (zeroed) holds 1, p,
// cls (+0x10), x+dx (+0x14), y+dy (+0x18), 10240(u) (+0x2C), 11029(u) (+0x30); 0xB30F0(ECX = block).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bb5b0(uint32_t p, uint32_t u, uint32_t cls, uint32_t f, uint32_t t, const cl::Env& e) {
    uint32_t row = e.Row((int32_t)cls);
    if (!row) return;
    int32_t a = e.R16s(row + 0x1e);
    if (a > 0) e.H(0xb7de0, p, (uint32_t)a, 0, 0, 0, t, 0, 0, 0, u);
    int32_t w34 = e.R16s(row + 0x34);
    if (w34 > 0 && f == 0x13b) e.H(0x88a70, p, 0, 0, 0, 0, 0, 0, 0, (uint32_t)w34);
    int32_t fi = (int32_t)f, ti = (int32_t)t;
    if (fi % 2 != 0) return;
    if (ti < 3 && fi % 16 != 0) return;
    int32_t n = ti < 3 ? 1 : ti < 6 ? 2 : ti < 9 ? 3 : 4;
    uint32_t dx = 0, dy = 0;
    if (n > 1) {
        uint32_t half = (uint32_t)(n / 2);
        dx = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)n) - half;
        dy = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)n) - half;
    }
    uint32_t q[23] = {};
    q[0] = 1; q[1] = p;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = cls;
    q[5] = e.PD(u, 0xc3be, 0xc) + dx;
    q[6] = e.PD(u, 0xc2ec, 0x10) + dy;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
