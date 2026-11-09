// 1.13c D2Client+0xC5670: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4); the unit is not read.
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. mis = short row+0xEC outside [0, count at +0xB6C) returns 0.
// p = #A5F70() == 0 returns 0. #4C500(EAX = 0, ECX = p, stack 3). Request block (23 dwords, zeroed): [1] = [2] = p, [4] = mis, [11] = index, [12] = a3;
// #B30F0(ECX = block). Returns 1 whatever #B30F0 returned.
#include "cl_env.h"
extern "C" uint32_t d2c_c5670(uint32_t, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t mis = e.R16s(row + 0xec);
    if (mis < 0 || mis >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    uint32_t p = e.H(0xa5f70);
    if (!p) return 0;
    e.H(0x4c500, 3, 0, 0, 0, 0, 0, p);
    uint32_t q[23] = {};
    q[1] = p; q[2] = p; q[4] = (uint32_t)mis; q[11] = (uint32_t)cls; q[12] = a3;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    return 1;
}
