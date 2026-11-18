// 1.13c D2Client+0xC5C70: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. A non-null unit gets word +0xC4 |= 0x40.
// m1 = short row+0xEA in [0, missile count at +0xB6C): #C4410(EAX = unit, stack m1, index, a3, 0).
// m2 = short row+0xEC in the same range: c = #C316(unit, row+0x114, index, a3); #C5730(EAX = unit, EDX = m2, stack c, index, a3). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c5c70(uint32_t unit, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    int32_t m1 = e.R16s(row + 0xea);
    if (m1 >= 0 && m1 < (int32_t)e.R(e.dt + 0xb6c))
        e.H(0xc4410, (uint32_t)m1, (uint32_t)cls, a3, 0, 0, unit);
    int32_t m2 = e.R16s(row + 0xec);
    if (m2 >= 0 && m2 < (int32_t)e.R(e.dt + 0xb6c)) {
        uint32_t c = e.H(0xc316, unit, e.R(row + 0x114), (uint32_t)cls, a3);
        e.H(0xc5730, c, (uint32_t)cls, a3, 0, 0, unit, 0, (uint32_t)m2);
    }
    return 1;
}
