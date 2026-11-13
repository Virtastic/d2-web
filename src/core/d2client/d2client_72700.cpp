// 1.13c D2Client+0x72700 -- skill effect replay: for a skill row, run the callee at 0x69660 over the two word ranges
// (+2..+4, +6..+8) with the unit in EDI. ECX = unit, EDX = skill. Returns the EAX the function leaves.
#include "cl_env.h"
extern "C" uint32_t d2c_72700(uint32_t unit, int32_t skill, const cl::Env& e) {
    if (skill < 0) return 0;
    const uint32_t dt = e.dt;
    if (skill >= (int32_t)e.R(dt + 0xc4)) return dt;
    const uint32_t row = (uint32_t)skill * 0x3c + e.R(dt + 0xbc);
    if (!row) return dt;
    uint32_t eax = dt;
    for (int part = 0; part < 2; part++) {
        const int32_t lo = e.R16s(row + 2 + 4 * part), hi = e.R16s(row + 4 + 4 * part);
        if (lo < 0 || lo > hi) continue;
        for (int32_t i = lo; i <= hi; i++) {
            eax = e.H(0x69660, 0, 0, 0, 0, 0, eax, 0, (uint32_t)i, 0, 0);
            e.Edj(0x69660, unit);
        }
    }
    return eax;
}
