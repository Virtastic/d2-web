// 1.13c D2Client+0xC5A80: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. mi = #40550(EAX = index, stack unit) outside [0, count at +0xB6C) returns 0.
// #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0. A non-null unit gets word +0xC4 |= 0x40. Request block (23 dwords, zeroed): [0] = 2,
// [1] = [2] = unit, [4] = mi, [7] = X - #1630(unit) = dx, [8] = Y - #1660(unit) = dy, [11] = index, [12] = a3. p = #B30F0(ECX = block) == 0 returns 0.
// #C448(p, -dy), #C394(p, dx). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c5a80(uint32_t unit, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t mi = (int32_t)e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)cls);
    if (mi < 0 || mi >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    uint32_t xy[2] = {};   // [0] = X (EDI), [1] = Y (stack word 0)
    e.Ed((uint32_t)(uintptr_t)&xy[0]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&xy[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    uint32_t q[23] = {};
    q[0] = 2; q[1] = unit; q[2] = unit; q[4] = (uint32_t)mi; q[11] = (uint32_t)cls; q[12] = a3;
    uint32_t dx = xy[0] - e.H(0x1630, 0, 0, 0, 0, 0, 0, unit);
    uint32_t dy = xy[1] - e.H(0x1660, 0, 0, 0, 0, 0, 0, unit);
    q[7] = dx; q[8] = dy;
    uint32_t p = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!p) return 0;
    e.H(0xc448, p, 0u - dy);
    e.H(0xc394, p, dx);
    return 1;
}
