// 1.13c D2Client+0x72770 -- skill row replay: a row that passes the bit-mask, word 0xE and word 0x18 range checks runs the
// callee at 0x69660 for three consecutive ids starting at word 0xE (unit in EDI). ECX = unit, EDX = skill.
#include "cl_env.h"
extern "C" uint32_t d2c_72770(uint32_t unit, int32_t skill, const cl::Env& e) {
    if (skill < 0) return 0;
    const uint32_t dt = e.dt;
    if (skill >= (int32_t)e.R(dt + 0xc4)) return dt;
    const uint32_t row = (uint32_t)skill * 0x3c + e.R(dt + 0xbc);
    if (!row) return dt;
    if (!(e.R8(e.bm + 0x10) & e.R8(row + 0x10))) return dt;
    const int32_t w = e.R16s(row + 0xe), v = e.R16s(row + 0x18);
    if (w <= 0 || w >= (int32_t)e.R(dt + 0xbc0)) return dt;
    if (v < 0 || v > (int32_t)e.R(dt + 0xbd4)) return dt;
    uint32_t eax = dt;
    for (int i = 0; i < 3; i++) {
        eax = e.H(0x69660, 0, 0, 0, 0, 0, eax, 0, (uint32_t)(w + i), 0, 0);
        e.Edj(0x69660, unit);
    }
    return eax;
}
